#include <stdint.h>
#include "hash_table.h"
#include <unistd.h>

/*
void initHashTable(HashTable* table, size_t initial_size, float load_factor, size_t (*hash_fn)(const void* key, size_t size), int (*cmp_fn)(const void* a, const void* b)) {
    table->items = (HashItem**)calloc(initial_size, sizeof(HashItem*));
    if (table->items == NULL) {
        fprintf(stderr, "哈希表的内存分配失败。\n");
        exit(EXIT_FAILURE);
    }
    table->size = initial_size;
    table->count = 0;
    table->load_factor = load_factor;
    table->hash = hash_fn;
    table->cmp = cmp_fn;
}

size_t defaultHash(const void* key, size_t size) {
    return (size_t)(uintptr_t)key % size;
}

int defaultCmp(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b) == 0;
}

char* processData(const char* key, const char* value) {
    size_t key_len = strlen(key);
    size_t value_len = strlen(value);
    char* processed_data = malloc(key_len + value_len + 2); // +2 for "=" and null terminator
    if (processed_data == NULL) {
        fprintf(stderr, "处理数据的内存分配失败。\n");
        exit(EXIT_FAILURE);
    }
    sprintf(processed_data, "%s=%s", key, value);
    return processed_data;
}

void* searchHashTable(HashTable* table, const void* key) {
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    while (item != NULL) {
        if (table->cmp(item->key, key)) {
            return item->value;
        }
        item = item->next;
    }
    return NULL;
}

void insertHashTable(HashTable* table, const char* key, const char* value) {
    table->count++;
    if (table->count > table->size * table->load_factor) {
        resizeHashTable(table, table->size * 2);
    }
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    if (item == NULL) {
        table->items[index] = (HashItem*)malloc(sizeof(HashItem));
        if (table->items[index] == NULL) {
            fprintf(stderr, "哈希项的内存分配失败。\n");
            exit(EXIT_FAILURE);
        }
        table->items[index]->key = strdup(key);
        table->items[index]->value = processData(key, value);
        table->items[index]->next = NULL;
    } else {
        while (item->next != NULL) {
            if (table->cmp(item->key, key)) {
                free(item->value);
                item->value = processData(key, value);
                return;
            }
            item = item->next;
        }
        if (table->cmp(item->key, key)) {
            free(item->value);
            item->value = processData(key, value);
        } else {
            item->next = (HashItem*)malloc(sizeof(HashItem));
            if (item->next == NULL) {
                fprintf(stderr, "哈希项的内存分配失败。\n");
                exit(EXIT_FAILURE);
            }
            item->next->key = strdup(key);
            item->next->value = processData(key, value);
            item->next->next = NULL;
        }
    }
}

void deleteHashTable(HashTable* table, const void* key) {
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    HashItem* prev = NULL;
    while (item != NULL) {
        if (table->cmp(item->key, key)) {
            if (prev == NULL) {
                table->items[index] = item->next;
            } else {
                prev->next = item->next;
            }
            free(item->value);
            free(item);
            table->count--;
            if (table->count < table->size * LOAD_FACTOR_SHRINK && table->size > TABLE_SIZE) {
                shrinkHashTable(table, table->size / 2);
            }
            return;
        }
        prev = item;
        item = item->next;
    }
}

void freeHashTable(HashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            HashItem* temp = item;
            item = item->next;
            // 移除 const 限定符
            free((void*)temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(table->items);
}

void resizeHashTable(HashTable* table, size_t new_size) {
    HashItem** new_items = (HashItem**)calloc(new_size, sizeof(HashItem*));
    if (new_items == NULL) {
        fprintf(stderr, "新哈希表的内存分配失败。\n");
        exit(EXIT_FAILURE);
    }
    rehashHashTable(table, new_size);
    free(table->items);
    table->items = new_items;
    table->size = new_size;
}

void rehashHashTable(HashTable* table, size_t new_size) {
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            HashItem* next = item->next;
            size_t new_index = table->hash(item->key, new_size);
            item->next = table->items[new_index];
            table->items[new_index] = item;
            item = next;
        }
    }
}

void shrinkHashTable(HashTable* table, size_t new_size) {
    if (new_size < TABLE_SIZE) return;
    resizeHashTable(table, new_size);
}

void handleMergeMessage(HashTable* table, Report* report) {
    report->data = NULL;
    report->length = 0;
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            size_t new_length = report->length + strlen(item->value);
            char* new_data = realloc(report->data, new_length + 1); // +1 for null terminator
            if (new_data == NULL) {
                fprintf(stderr, "组装消息的内存分配失败。\n");
                exit(EXIT_FAILURE);
            }
            report->data = new_data;
            strcpy(report->data + report->length, item->value);
            report->length = new_length;
            item = item->next;
        }
    }
}
*/

size_t strLength(const char* str) {
    const char* s;
    for (s = str; *s; ++s) {
    }
    return (size_t)(s - str);
}

void handleError(const char* msg) {
    const char* error_msg = "Error: ";
    write(1, error_msg, strLength(error_msg));
    write(1, msg, strLength(msg));
    write(1, "\n", 1);
    exit(1);
}

char* strCopy(const char* src) {
    size_t len = strLength(src) + 1;
    char* dst = (char*)malloc(len);
    if (dst == NULL) {
        handleError("内存分配失败");
    }
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
    return dst;
}

int strCmp(const char* a, const char* b) {
    while (*a != '\0' && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

void initHashTable(HashTable* table, size_t initial_size, float load_factor,
                   size_t (*hash_fn)(const void* key, size_t size),
                   int (*cmp_fn)(const void* a, const void* b)) {
    table->items = (HashItem**)calloc(initial_size, sizeof(HashItem*));
    if (table->items == NULL) {
        handleError("哈希表的内存分配失败");
    }
    table->size = initial_size;
    table->count = 0;
    table->load_factor = load_factor;
    table->hash = hash_fn;
    table->cmp = cmp_fn;
}

size_t defaultHash(const void* key, size_t size) {
    return (size_t)(uintptr_t)key % size;
}

int defaultCmp(const void* a, const void* b) {
    return strCmp((const char*)a, (const char*)b) == 0;
}

char* processData(const char* key, const char* value) {
    size_t key_len = strLength(key);
    size_t value_len = strLength(value);
    char* processed_data = (char*)malloc(key_len + value_len + 2); // +2 for "=" and null terminator
    if (processed_data == NULL) {
        handleError("处理数据的内存分配失败");
    }
    for (size_t i = 0; i < key_len; ++i) {
        processed_data[i] = key[i];
    }
    processed_data[key_len] = '=';
    for (size_t i = 0; i < value_len; ++i) {
        processed_data[key_len + 1 + i] = value[i];
    }
    processed_data[key_len + value_len + 1] = '\0';
    return processed_data;
}

void* searchHashTable(HashTable* table, const void* key) {
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    while (item != NULL) {
        if (table->cmp(item->key, key) == 0) { 
            return item->value;
        }
        item = item->next;
    }
    return NULL;
}

void insertHashTable(HashTable* table, const char* key, const char* value) {
    table->count++;
    if (table->count > table->size * table->load_factor) {
        resizeHashTable(table, table->size * 2);
    }
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    if (item == NULL) {
        table->items[index] = (HashItem*)malloc(sizeof(HashItem));
        if (table->items[index] == NULL) {
            handleError("哈希项的内存分配失败");
        }
        table->items[index]->key = strCopy(key);
        table->items[index]->value = processData(key, value);
        table->items[index]->next = NULL;
    } else {
        while (item->next != NULL) {
            if (table->cmp(item->key, key) == 0) {
                free(item->value);
                item->value = processData(key, value);
                return;
            }
            item = item->next;
        }
        if (table->cmp(item->key, key) == 0) {
            free(item->value);
            item->value = processData(key, value);
        } else {
            item->next = (HashItem*)malloc(sizeof(HashItem));
            if (item->next == NULL) {
                handleError("哈希项的内存分配失败");
            }
            item->next->key = strCopy(key);
            item->next->value = processData(key, value);
            item->next->next = NULL;
        }
    }
}

void deleteHashTable(HashTable* table, const void* key) {
    size_t index = table->hash(key, table->size);
    HashItem* item = table->items[index];
    HashItem* prev = NULL;
    while (item != NULL) {
        if (table->cmp(item->key, key) == 0) { 
            if (prev == NULL) {
                table->items[index] = item->next;
            } else {
                prev->next = item->next;
            }
            free(item->value);
            free(item); 
            table->count--;
            if (table->count < table->size * LOAD_FACTOR_SHRINK && table->size > TABLE_SIZE) {
                shrinkHashTable(table, table->size / 2);
            }
            return;
        }
        prev = item;
        item = item->next;
    }
}

void freeHashTable(HashTable* table) {
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            HashItem* temp = item;
            item = item->next;
            free((void*)temp->key);  // 去除 const 限定符
            free(temp->value);
            free(temp);
        }
    }
    free(table->items);
}

void resizeHashTable(HashTable* table, size_t new_size) {
    HashItem** new_items = (HashItem**)calloc(new_size, sizeof(HashItem*));
    if (new_items == NULL) {
        handleError("新哈希表的内存分配失败");
    }
    rehashHashTable(table, new_size);
    free(table->items);
    table->items = new_items;
    table->size = new_size;
}

void rehashHashTable(HashTable* table, size_t new_size) {
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            HashItem* next = item->next;
            size_t new_index = table->hash(item->key, new_size);
            item->next = table->items[new_index];
            table->items[new_index] = item;
            item = next;
        }
    }
}

void shrinkHashTable(HashTable* table, size_t new_size) {
    if (new_size < TABLE_SIZE) return;
    resizeHashTable(table, new_size);
}

void handleMergeMessage(HashTable* table, Report* report) {
    report->data = NULL;
    report->length = 0;
    for (size_t i = 0; i < table->size; i++) {
        HashItem* item = table->items[i];
        while (item != NULL) {
            size_t new_length = report->length + strLength((const char*)item->value);
            char* new_data = (char*)realloc(report->data, new_length + 1); // +1 for null terminator
            if (new_data == NULL) {
                handleError("组装消息的内存分配失败");
            }
            report->data = new_data;
            for (size_t j = 0; j < new_length - report->length; ++j) {
                report->data[report->length + j] = ((char*)item->value)[j];
            }
            report->length = new_length;
            item = item->next;
        }
    }
}