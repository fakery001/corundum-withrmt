#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"

int main() {
   /*
    HashTable table;
    initHashTable(&table);
    insertHashTable(&table, 100, "Value for key 100");
    insertHashTable(&table, 200, "Value for key 200");
    insertHashTable(&table, 300, "Value for key 300");
    insertHashTable(&table, 400, "Value for key 400");
    void* value = searchHashTable(&table, 300);
    if (value != NULL) {
        printf("Found value for key 300: %s\n", (char*)value);
    } else {
        printf("Value for key 300 not found.\n");
    }
    value = searchHashTable(&table, 400);
    if (value != NULL) {
        printf("Found value for key 400: %s\n", (char*)value);
    } else {
        printf("Value for key 400 not found.\n");
    }
    freeHashTable(&table);
    return 0;
    */

    HashTable table;
    initHashTable(&table, TABLE_SIZE, LOAD_FACTOR_EXPAND, defaultHash, defaultCmp);
    const char* key1 = "key1";
    const char* value1 = "value1";
    insertHashTable(&table, key1, strdup(value1)); 
    const char* key2 = "key2";
    const char* value2 = "value2";
    insertHashTable(&table, key2, strdup(value2));
    const char* key3 = "key3";
    const char* value3 = "value3";
    insertHashTable(&table, key3, strdup(value3));
    const char* key4 = "key4";
    const char* value4 = "value4";
    insertHashTable(&table, key4, strdup(value4));
    void* foundValue1 = searchHashTable(&table, key1);
    if (foundValue1 != NULL) {
        printf("Found value for key1: %s\n", (char*)foundValue1);
    }
    void* foundValue2 = searchHashTable(&table, key2);
    if (foundValue2 != NULL) {
        printf("Found value for key2: %s\n", (char*)foundValue2);
    }
    insertHashTable(&table, key1, strdup("new_value1")); 
    void* foundValue1Updated = searchHashTable(&table, key1);
    if (foundValue1Updated != NULL) {
        printf("Updated value for key1: %s\n", (char*)foundValue1Updated);
    }
    deleteHashTable(&table, key1);
    void* deletedValue = searchHashTable(&table, key1);
    if (deletedValue == NULL) {
        printf("Key1 has been deleted.\n");
    }
    // 处理合并报文，搜索报表数据
    Report report;
    report.data = "report_data";
    report.length = strlen(report.data);
    handleMergeMessage(&table, &report);
    void* foundReportData = searchHashTable(&table, report.data);
    if (foundReportData != NULL) {
        printf("Found report data: %s\n", (char*)foundReportData);
    }
    freeHashTable(&table);
    return 0;

}

void irqCallback(){
}
