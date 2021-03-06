#include "HashTable.h"

Elem :: Elem (__m256i* key, const char* value):
    key_   (key),
    value_ (value)
{
    // assert (value);
}

Elem :: ~Elem()
{
    if (this)
    {
        delete next_;
        next_  = nullptr;
        key_   = nullptr;
        value_ = nullptr;
    }
    return;
}

HashTable :: HashTable (size_t size, hash_t (*hash_func) (__m256i*)):
    table_      ((Elem**) calloc (size, sizeof (*table_))),
    table_size_ (size),
    hash_func_  (hash_func)
{
    // ToDo: Simple size
}

HashTable :: ~HashTable ()
{
    // for (size_t i_elem = 0; i_elem < table_size_; i_elem++)
    //     delete table_[i_elem];
    table_size_ = 0;
    free (table_);
    free (values_);
    free (elem_data_);
    free (keys_);
}

void HashTable :: FillTable ()
{
    size_t* pointers = nullptr;

    // ToDo: Delete copypaste

    size_t elem_num = ReadFile ((char**) &keys_, "Database/intrinsics");
    elem_num /= 32;
    if (!elem_num)
    {
        printf ("Error: Can't open \"Database/intrinsics\"\n");
        return;
    }

    if (!ReadFile (&values_, "Database/values"))
    {
        printf ("Error: Can't open \"Database/values\"\n");
        free (keys_);
        return;
    }

    if (!ReadFile ((char**) &pointers, "Database/pointers"))
    {
        printf ("Error: Can't open \"Database/pointers\"\n");
        free (values_);
        free (keys_);
        return;
    }

    elem_data_ = (Elem*) calloc (elem_num, sizeof(*elem_data_));

    for (size_t i_elem = 0; i_elem < elem_num; i_elem++)
        AddKey (elem_data_ + i_elem, keys_ + i_elem, values_ + pointers[i_elem]);

    free (pointers);
}

inline void HashTable :: AddKey (Elem* elem, __m256i* key, const char* value)
{
    // assert (key);
    // assert (value);
    
    // Elem* elem        = new Elem (key, value);
    elem->key_   = key;
    elem->value_ = value;
    hash_t position   = hash_func_ (key) % table_size_;
    elem->elem_number = table_[position] ? table_[position]->elem_number + 1 : 1;
    elem->next_       = table_[position];
    table_[position]  = elem;
}

void HashTable :: GetStatistic ()
{
    if (!table_)
    {
        printf ("Can't get statistic: table wasn't created.\n");
        return;
    }

    printf ("Get statistic: Creating data...\n");

    FILE* stat = fopen ("Statistic.csv", "w");

    double E_hash = 0, E2_hash = 0;

    for (size_t i_elem = 0; i_elem < table_size_; i_elem++)
    {
        size_t size = table_[i_elem] ? table_[i_elem]->elem_number : 0;
        E_hash  += size;
        E2_hash += size * size;
        fprintf (stat, "%lu,", size);
    }

    E_hash  /= table_size_;
    E2_hash /= table_size_;

    printf ("Get statistic: D = %lf\n", E2_hash - E_hash * E_hash);

    fseek (stat, -1, SEEK_CUR);
    fprintf (stat, "\n");
    fclose (stat);

    printf ("Get statistic: Creating graphic...\n");
    system ("python3 Graphic.py");
    printf ("Get statistic: Complete!\n");

    return;
}

size_t ReadFile (char** text, const char* file_name)
{
    assert (text);
	assert (file_name);

	FILE* file = fopen (file_name, "rb");
	if (!file)
	{
		printf ("[Input error] Unable to open file \"%s\"\n", file_name);
		return 0;
	}

    size_t num_symbols = GetFileSize (file);

	*text = new char[(num_symbols + 1) * sizeof(**text)];

	if (!text)
	{
		printf ("[Error] Unable to allocate memory\n");
        fclose (file);
		return 0;
	}

	if (fread (*text, sizeof(**text), num_symbols, file) != num_symbols)
    {
	    fclose (file);
        return 0;
    }

    fclose (file);
	return num_symbols;
}

size_t GetFileSize (FILE* file)
{
    // ToDo: Work with files, that pointer != start
    assert (file);

    fseek (file, 0, SEEK_END);
	size_t num_symbols = ftell (file);
	fseek (file, 0, SEEK_SET);

    return num_symbols;
}

const char* HashTable :: Find (__m256i key)
{
    Elem* list = table_[hash_func_ (&key) % table_size_];

    while (list != nullptr)
    {
        if (_mm256_testc_si256 (key, _mm256_loadu_si256(list->key_)))
            return list->value_;
        
        list = list->next_;
    }

    return nullptr;
}
