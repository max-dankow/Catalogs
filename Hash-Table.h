struct Node;
struct Table;

size_t Hash(const char *key, const size_t size, const long long prime_local);
struct Table *create_table(const size_t size, const long long new_prime);
void write_table(const struct Table *table);
struct Node* list_add(struct Node* inz, const char *key, const void *new_value_ptr);
struct Table *add_element(struct Table *table, const char *key, const void *new_value_ptr);
int find_element(struct Table *table, const char *key, void *dst);
void remove_element(struct Table *table, const char *key);
void dispose_table(struct Table **table, int dispose_all_data);

