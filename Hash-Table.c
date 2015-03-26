#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Hash-Table.h"

typedef struct Node
{
    struct Node *next;
    char *key;
    void *value_ptr;
} Node;

typedef struct Table
{
    size_t size;
    size_t element_number;
    long long prime;
    Node **buckets;
} Table;

size_t Hash(const char *key, const size_t size, const long long prime_local)
{
    long long current_hash = 0, pow = 1;

    for (; *key != '\0'; ++key)
    {
        current_hash = (current_hash + pow * ((unsigned char) (*key))) % size;
        assert(current_hash >= 0);
        pow = (pow * prime_local) % size;
    }

    return (size_t) (current_hash % size);
}

Table *create_table(const size_t size, const long long new_prime)
{
    Table* new_table = malloc(sizeof(Table));
    new_table->size = size;
    new_table->prime = new_prime;
    new_table->element_number = 0;
    new_table->buckets = malloc(size * sizeof(Node));

    for (size_t i = 0; i < size; ++i)
    {
        new_table->buckets[i] = NULL;
    }

    return new_table;
}

void write_table(const Table *table)
{
    if (table == NULL)
        return;

    for (size_t i = 0; i < table->size; ++i)
    {
        Node *current_node = table->buckets[i];
        while (current_node != NULL)
        {
            printf("%s ", current_node->key);
            current_node = current_node->next;
        }
        printf("NULL\n");
    }
}

Node* list_add(Node* inz, const char *key, const void *new_value_ptr)
{
    Node *new_node = malloc(sizeof(Node));
    new_node->next = inz;
    new_node->key = key;
    new_node->value_ptr = new_value_ptr;

    return new_node;
}

void copy_table(Table *src, Table *dst)
{
    if (src == NULL || dst == NULL)
        return;

    for (size_t i = 0; i < src->size; ++i)
    {
        Node *current_node = src->buckets[i];
        while (current_node != NULL)
        {
            add_element(dst, current_node->key, current_node->value_ptr);
            current_node = current_node->next;
        }
    }
}

Table *rehash(Table *table)
{
    if (table->element_number >= (4/3)*table->size)
    {
        Table *new_table = create_table(table->size * 2, table->prime);
        copy_table(table, new_table);
        dispose_table(&table, 0);//0 - оставить value и key в памяти
        printf("\033[31mTable was rehashed. New size is %d\n\033[0m", new_table->size);
        return new_table;
    }
    else
    {
        return table;
    }
}

Table *add_element(Table *table, const char *key, const void *new_value_ptr)
{
    if (table == NULL)
        return table;

    size_t hash_key = Hash(key, table->size, table->prime);
    Node *inz = table->buckets[hash_key];
    table->element_number++;
    table->buckets[hash_key] = list_add(inz, key, new_value_ptr);

    return rehash(table);
}

int find_element(Table *table, const char *key, void *dst)
{
    dst = NULL;

    if (table == NULL)
        return -1;

    size_t hash_key = Hash(key,table->size, table->prime);
    Node *inz = table->buckets[hash_key];

    while ((inz != NULL) && (strcmp(inz->key, key) != 0))
        inz = inz->next;

    if (inz == NULL)
    {
        dst = NULL;
        return -1;
    }

    dst = inz->value_ptr;
    return 0;
}

void remove_element(Table *table, const char *key)
{
    if (table == NULL)
        return;

    size_t hash_key = Hash(key,table->size, table->prime);
    Node *inz = table->buckets[hash_key];

    if (inz == NULL)
        return;

    if (strcmp(inz->key, key) == 0)
    {
        Node *temp = inz->next;
        free(inz->key);
        free(inz->value_ptr);
        free(inz);
        table->element_number--;
        table->buckets[hash_key] = temp;
        return;
    }

    while ((inz->next != NULL) && (strcmp(inz->next->key, key) != 0))
        inz = inz->next;

    if ((inz->next != NULL) && (strcmp(inz->next->key, key) == 0))
    {
        Node *temp = inz->next->next;

        if (inz->next->key != NULL)
        {
            free(inz->next->key);
            inz->next->key = NULL;
        }

        if (inz->next->value_ptr != NULL)
        {
            free(inz->next->value_ptr);
            inz->next->value_ptr = NULL;
        }

        free(inz->next);
        table->element_number--;
        inz->next = temp;
    }

}

void dispose_table(Table **table, int dispose_all_data)
{
    for (size_t i = 0; i < (*table)->size; ++i)
    {
        Node *current_ptr = (*table)->buckets[i];
        while (current_ptr != NULL)
        {
            Node *deleted_ptr = current_ptr;
            current_ptr = current_ptr->next;

            if (dispose_all_data == 1)
            {
                if (deleted_ptr->key != NULL)
                {
                    free(deleted_ptr->key);
                    deleted_ptr->key = NULL;
                }

                if (deleted_ptr->value_ptr != NULL)
                {
                    free(deleted_ptr->value_ptr);
                    deleted_ptr->value_ptr = NULL;
                }
            }

            free(deleted_ptr);
        }
    }

    free((*table)->buckets);
    free(*table);
    *table = NULL;
}
