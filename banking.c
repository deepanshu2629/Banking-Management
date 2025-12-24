#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

/* ============================================================================
   DEFINITIONS & CONSTANTS
   ============================================================================ */

#define EMP_FILE "employees.txt"
#define CUST_FILE "customers.txt"

#define MAX_LINE 1024
#define MAX_NAME 100
#define MAX_DESIGN 50
#define MAX_ADDR 200
#define MAX_AAD 14
#define MAX_PHONE 12

/* ============================================================================
   STRUCTURES
   ============================================================================ */

typedef struct {
    int id;
    char name[MAX_NAME];
    char salary[32];
    char designation[MAX_DESIGN];
} Employee;

typedef struct {
    int account;
    char name[MAX_NAME];
    char aadhaar[MAX_AAD];
    char phone[MAX_PHONE];
    long balance;
    char address[MAX_ADDR];
} Customer;

/* ============================================================================
   UTILITY FUNCTIONS
   ============================================================================ */

/* Validate numeric string (digits only) */
static int is_numeric(const char *str) {
    if (!str || *str == '\0') return 0;
    for (const char *p = str; *p; p++) {
        if (!isdigit((unsigned char)*p)) return 0;
    }
    return 1;
}

/* Validate alphabetic string (letters and spaces only) */
static int is_alphabetic(const char *str) {
    if (!str || *str == '\0') return 0;
    for (const char *p = str; *p; p++) {
        if (!isalpha((unsigned char)*p) && *p != ' ') return 0;
    }
    return 1;
}

/* utility: trim newline and whitespace */
static void trim_newline(char *s) {
    if (!s) return;
    size_t l = strlen(s);
    while (l > 0 && (s[l-1] == '\n' || s[l-1] == '\r')) {
        s[l-1] = '\0';
        l--;
    }
}

/* Ensure file exists */
void ensure_file_exists(const char *path) {
    FILE *f = fopen(path, "a");
    if (f) fclose(f);
}

/* Helper input utility */
static void read_line_input(const char *prompt, char *buf, size_t sz) {
    if (prompt) printf("%s", prompt);
    fflush(stdout);  // Ensure prompt is displayed
    if (fgets(buf, (int)sz, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
    
    // Clear any remaining input in buffer if line was too long
    if (strlen(buf) == sz-1 && buf[sz-2] != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

/* ============================================================================
   EMPLOYEE FILE OPERATIONS
   ============================================================================ */

int load_employees(Employee **out, int *count) {
    ensure_file_exists(EMP_FILE);
    FILE *f = fopen(EMP_FILE, "r");
    if (!f) return -1;
    Employee *arr = NULL;
    int cap = 0, n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (line[0] == '\0') continue;
        /* Format: id|name|salary|designation */
        char *p = line;
        char *parts[4] = {0};
        int idx = 0;
        parts[idx++] = p;
        while (*p && idx < 4) {
            if (*p == '|') { *p = '\0'; parts[idx++] = p+1; }
            p++;
        }
        if (idx < 4) continue;
        if (n == cap) {
            cap = cap ? cap * 2 : 8;
            arr = realloc(arr, cap * sizeof(Employee));
        }
        Employee e;
        e.id = atoi(parts[0]);
        strncpy(e.name, parts[1], MAX_NAME-1); e.name[MAX_NAME-1] = '\0';
        strncpy(e.salary, parts[2], sizeof(e.salary)-1); e.salary[sizeof(e.salary)-1] = '\0';
        strncpy(e.designation, parts[3], MAX_DESIGN-1); e.designation[MAX_DESIGN-1] = '\0';
        arr[n++] = e;
    }
    fclose(f);
    *out = arr;
    *count = n;
    return 0;
}

int save_employees(const Employee *emps, int count) {
    FILE *f = fopen(EMP_FILE, "w");
    if (!f) return -1;
    for (int i = 0; i < count; ++i) {
        fprintf(f, "%d|%s|%s|%s\n", emps[i].id, emps[i].name, emps[i].salary, emps[i].designation);
    }
    fclose(f);
    return 0;
}

int append_employee(const Employee *e) {
    ensure_file_exists(EMP_FILE);
    FILE *f = fopen(EMP_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%d|%s|%s|%s\n", e->id, e->name, e->salary, e->designation);
    fclose(f);
    return 0;
}

/* ============================================================================
   CUSTOMER FILE OPERATIONS
   ============================================================================ */

int load_customers(Customer **out, int *count) {
    ensure_file_exists(CUST_FILE);
    FILE *f = fopen(CUST_FILE, "r");
    if (!f) return -1;
    Customer *arr = NULL;
    int cap = 0, n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (line[0] == '\0') continue;
        /* Format: account|name|aadhaar|phone|balance|address */
        char *p = line;
        char *parts[6] = {0};
        int idx = 0;
        parts[idx++] = p;
        while (*p && idx < 6) {
            if (*p == '|') { *p = '\0'; parts[idx++] = p+1; }
            p++;
        }
        if (idx < 6) continue;
        if (n == cap) {
            cap = cap ? cap * 2 : 8;
            arr = realloc(arr, cap * sizeof(Customer));
        }
        Customer c;
        c.account = atoi(parts[0]);
        strncpy(c.name, parts[1], MAX_NAME-1); c.name[MAX_NAME-1] = '\0';
        strncpy(c.aadhaar, parts[2], MAX_AAD-1); c.aadhaar[MAX_AAD-1] = '\0';
        strncpy(c.phone, parts[3], MAX_PHONE-1); c.phone[MAX_PHONE-1] = '\0';
        c.balance = atol(parts[4]);
        strncpy(c.address, parts[5], MAX_ADDR-1); c.address[MAX_ADDR-1] = '\0';
        arr[n++] = c;
    }
    fclose(f);
    *out = arr;
    *count = n;
    return 0;
}

int save_customers(const Customer *custs, int count) {
    FILE *f = fopen(CUST_FILE, "w");
    if (!f) return -1;
    for (int i = 0; i < count; ++i) {
        fprintf(f, "%d|%s|%s|%s|%ld|%s\n",
                custs[i].account, custs[i].name, custs[i].aadhaar, custs[i].phone, custs[i].balance, custs[i].address);
    }
    fclose(f);
    return 0;
}

int append_customer(const Customer *c) {
    ensure_file_exists(CUST_FILE);
    FILE *f = fopen(CUST_FILE, "a");
    if (!f) return -1;
    fprintf(f, "%d|%s|%s|%s|%ld|%s\n", c->account, c->name, c->aadhaar, c->phone, c->balance, c->address);
    fclose(f);
    return 0;
}

/* ============================================================================
   PRINT FUNCTIONS
   ============================================================================ */

static void print_employees(const Employee *emps, int count) {
    if (count == 0) {
        printf("\n\tData file was empty\n");
        return;
    }
    printf("\n--- Employees (%d) ---\n", count);
    printf("%-6s | %-20s | %-10s | %-12s\n", "ID", "Name", "Salary", "Designation");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < count; ++i) {
        printf("%-6d | %-20s | %-10s | %-12s\n", emps[i].id, emps[i].name, emps[i].salary, emps[i].designation);
    }
}

static void print_customers(const Customer *custs, int count) {
    if (count == 0) {
        printf("\n\tData file was empty\n");
        return;
    }
    printf("\n--- Customers (%d) ---\n", count);
    printf("%-6s | %-20s | %-12s | %-10s | %-10s | %-20s\n", "Acc", "Name", "aadhaar", "Phone", "Balance", "Address");
    printf("-----------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; ++i) {
        printf("%-6d | %-20s | %-12s | %-10s | %-10ld | %-20s\n",
               custs[i].account, custs[i].name, custs[i].aadhaar, custs[i].phone, custs[i].balance, custs[i].address);
    }
}

/* ============================================================================
   MENU OPERATIONS
   ============================================================================ */

void create_new() {
    printf("\n\t1. Employee\n\t2. Customer\n");
    char buf[32];
    read_line_input("\n\tEnter your choice: ", buf, sizeof(buf));
    int ch = atoi(buf);
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        int id = count ? (emps[count-1].id + 1) : 1;
        free(emps);
        Employee e;
        e.id = id;
        
        /* Validate employee name - must be alphabetic */
        while (1) {
            read_line_input("\n\tEnter employee name: ", e.name, sizeof(e.name));
            if (strlen(e.name) == 0) {
                printf("\n\tName cannot be empty\n");
                continue;
            }
            if (!is_alphabetic(e.name)) {
                printf("\n\tInvalid name - must contain only letters and spaces\n");
                continue;
            }
            break;
        }
        
        /* Validate salary - must be numeric */
        while (1) {
            read_line_input("\n\tEnter employee salary: ", e.salary, sizeof(e.salary));
            if (strlen(e.salary) == 0) {
                printf("\n\tSalary cannot be empty\n");
                continue;
            }
            if (!is_numeric(e.salary)) {
                printf("\n\tInvalid salary - must contain only digits\n");
                continue;
            }
            break;
        }
        
        /* Validate designation - must be alphabetic */
        while (1) {
            read_line_input("\n\tEnter employee designation: ", e.designation, sizeof(e.designation));
            if (strlen(e.designation) == 0) {
                printf("\n\tDesignation cannot be empty\n");
                continue;
            }
            if (!is_alphabetic(e.designation)) {
                printf("\n\tInvalid designation - must contain only letters and spaces\n");
                continue;
            }
            break;
        }
        
        append_employee(&e);
        printf("\n\tEmployee saved. ID: %d\n", e.id);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        int acc = count ? (custs[count-1].account + 1) : 1;
        free(custs);
        Customer c;
        c.account = acc;
        char balbuf[64];
        while (1) {
            /* Validate customer name - must be alphabetic */
            read_line_input("\n\tEnter customer name: ", c.name, sizeof(c.name));
            if (strlen(c.name) == 0) { 
                printf("\n\tName cannot be empty\n"); 
                continue; 
            }
            if (!is_alphabetic(c.name)) {
                printf("\n\tInvalid name - must contain only letters and spaces\n");
                continue;
            }
            
            /* Validate aadhaar - exactly 12 digits */
            read_line_input("\n\tEnter aadhaar (12 digits): ", c.aadhaar, sizeof(c.aadhaar));
            if (strlen(c.aadhaar) != 12 || !is_numeric(c.aadhaar)) { 
                printf("\n\tInvalid aadhaar - must be exactly 12 digits\n"); 
                continue;
            }
            
            /* Validate phone - exactly 10 digits */
            read_line_input("\n\tEnter phone (10 digits): ", c.phone, sizeof(c.phone));
            if (strlen(c.phone) != 10 || !is_numeric(c.phone)) { 
                printf("\n\tInvalid phone - must be exactly 10 digits\n"); 
                continue;
            }
            
            /* Validate initial deposit - must be numeric and >= 1000 */
            read_line_input("\n\tEnter initial deposit (min 1000 & max 50000): ", balbuf, sizeof(balbuf));
            if (!is_numeric(balbuf)) {
                printf("\n\tInvalid amount - must contain only digits\n");
                continue;
            }
            long bal = atol(balbuf);
            if (bal < 1000) { 
                printf("\n\tDeposit must be at least 1000\n"); 
                continue;
            }
            if (bal > 50000) { 
                printf("\n\tDeposit cannot exceed 50000\n"); 
                continue;
            }
            c.balance = bal;
            
            /* Validate address - cannot be empty */
            read_line_input("\n\tEnter address: ", c.address, sizeof(c.address));
            if (strlen(c.address) == 0) { 
                printf("\n\tAddress cannot be empty\n"); 
                continue; 
            }
            
            append_customer(&c);
            printf("\n\tCustomer saved. Account: %d  Balance: %ld\n", c.account, c.balance);
            break;
        }
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void view_all() {
    printf("\n\t1. View Employees\n\t2. View Customers\n");
    char buf[32];
    read_line_input("\n\tEnter choice: ", buf, sizeof(buf));
    int ch = atoi(buf);
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(emps);
            return;
        }
        read_line_input("\n\t1. Ascending\n\t2. Descending\n\tEnter: ", buf, sizeof(buf));
        int order = atoi(buf);
        
        if (order == 2) {
            /* print reversed */
            Employee *rev = malloc(count * sizeof(Employee));
            for (int i = 0; i < count; ++i) rev[i] = emps[count-1-i];
            print_employees(rev, count);
            free(rev);
        } else {
            print_employees(emps, count);
        }
        free(emps);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(custs);
            return;
        }
        read_line_input("\n\t1. Ascending\n\t2. Descending\n\tEnter: ", buf, sizeof(buf));
        int order = atoi(buf);
        
        if (order == 2) {
            Customer *rev = malloc(count * sizeof(Customer));
            for (int i = 0; i < count; ++i) rev[i] = custs[count-1-i];
            print_customers(rev, count);
            free(rev);
        } else {
            print_customers(custs, count);
        }
        free(custs);
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void search_data() {
    printf("\n\t1. Search Employee\n\t2. Search Customer\n");
    char buf[64];
    read_line_input("\n\tEnter choice: ", buf, sizeof(buf));
    int ch = atoi(buf);
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(emps);
            return;
        }
        printf("\n\t1. By Designation\n\t2. By Name\n\t3. By ID\n");
        read_line_input("\n\tEnter: ", buf, sizeof(buf));
        int opt = atoi(buf);
        
        
        if (opt == 1) {
            read_line_input("\n\tEnter designation: ", buf, sizeof(buf));
            char key[MAX_DESIGN]; strncpy(key, buf, sizeof(key));
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (strcasecmp(emps[i].designation, key) == 0) {
                    print_employees(&emps[i], 1);
                    found++;
                }
            }
            if (!found) printf("\n\tNo employee found.\n");
        } else if (opt == 2) {
            read_line_input("\n\tEnter name: ", buf, sizeof(buf));
            char key[MAX_NAME]; strncpy(key, buf, sizeof(key));
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (strcasecmp(emps[i].name, key) == 0) {
                    print_employees(&emps[i], 1);
                    found++;
                }
            }
            if (!found) printf("\n\tNo employee found.\n");
        } else if (opt == 3) {
            read_line_input("\n\tEnter ID: ", buf, sizeof(buf));
            int id = atoi(buf);
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (emps[i].id == id) {
                    print_employees(&emps[i], 1);
                    found = 1; break;
                }
            }
            if (!found) printf("\n\tNo employee found.\n");
        } else {
            printf("\n\tInvalid option\n");
        }
        free(emps);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(custs);
            return;
        }
        printf("\n\t1. By Account\n\t2. By aadhaar\n\t3. By Phone\n");
        char buf2[64];
        read_line_input("\n\tEnter: ", buf2, sizeof(buf2));
        int opt = atoi(buf2);
        
        if (opt == 1) {
            read_line_input("\n\tEnter account number: ", buf2, sizeof(buf2));
            int acc = atoi(buf2);
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (custs[i].account == acc) { print_customers(&custs[i], 1); found = 1; break; }
            }
            if (!found) printf("\n\tNo customer found.\n");
        } else if (opt == 2) {
            read_line_input("\n\tEnter aadhaar: ", buf2, sizeof(buf2));
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (strcmp(custs[i].aadhaar, buf2) == 0) { print_customers(&custs[i], 1); found = 1; break; }
            }
            if (!found) printf("\n\tNo customer found.\n");
        } else if (opt == 3) {
            read_line_input("\n\tEnter phone: ", buf2, sizeof(buf2));
            int found = 0;
            for (int i = 0; i < count; ++i) {
                if (strcmp(custs[i].phone, buf2) == 0) { print_customers(&custs[i], 1); found = 1; break; }
            }
            if (!found) printf("\n\tNo customer found.\n");
        } else {
            printf("\n\tInvalid option\n");
        }
        free(custs);
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void delete_data() {
    printf("\n\t1. Employee\n\t2. Customer\n");
    char buf[64];
    read_line_input("\n\tEnter choice: ", buf, sizeof(buf));
    int ch = atoi(buf);
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        if (count == 0) { printf("\n\tData file was empty\n"); free(emps); return; }
        printf("\n\t1. By ID\n\t2. By Name\n\t3. By Designation\n\t4. Delete all\n");
        read_line_input("\n\tEnter option: ", buf, sizeof(buf));
        int opt = atoi(buf);
        
        if (opt == 4) {
            read_line_input("\n\tAre you sure to delete all? (YES/NO): ", buf, sizeof(buf));
            if (strcasecmp(buf, "YES") == 0) { save_employees(NULL, 0); printf("\n\tAll deleted.\n"); }
            else printf("\n\tCancelled.\n");
        } else {
            Employee *newarr = malloc(count * sizeof(Employee));
            int newc = 0, removed = 0;
            if (opt == 1) {
                read_line_input("\n\tEnter ID to delete: ", buf, sizeof(buf));
                int id = atoi(buf);
                for (int i = 0; i < count; ++i) {
                    if (emps[i].id == id) { removed++; continue; }
                    newarr[newc++] = emps[i];
                }
            } else if (opt == 2) {
                read_line_input("\n\tEnter name to delete: ", buf, sizeof(buf));
                for (int i = 0; i < count; ++i) {
                    if (strcasecmp(emps[i].name, buf) == 0) { removed++; continue; }
                    newarr[newc++] = emps[i];
                }
            } else if (opt == 3) {
                read_line_input("\n\tEnter designation to delete: ", buf, sizeof(buf));
                for (int i = 0; i < count; ++i) {
                    if (strcasecmp(emps[i].designation, buf) == 0) { removed++; continue; }
                    newarr[newc++] = emps[i];
                }
            } else {
                printf("\n\tInvalid option\n");
                free(newarr); free(emps);
                return;
            }
            if (removed == 0) printf("\n\tNo matching records found.\n");
            save_employees(newarr, newc);
            printf("\n\tDeleted %d records.\n", removed);
            free(newarr);
        }
        free(emps);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        if (count == 0) { printf("\n\tData file was empty\n"); free(custs); return; }
        printf("\n\t1. By Account\n\t2. By Name\n\t3. By aadhaar\n\t4. Delete all\n");
        read_line_input("\n\tEnter option: ", buf, sizeof(buf));
        int opt = atoi(buf);
        
        if (opt == 4) {
            read_line_input("\n\tAre you sure to delete all? (YES/NO): ", buf, sizeof(buf));
            if (strcasecmp(buf, "YES") == 0) { save_customers(NULL, 0); printf("\n\tAll deleted.\n"); }
            else printf("\n\tCancelled.\n");
        } else {
            Customer *newarr = malloc(count * sizeof(Customer));
            int newc = 0, removed = 0;
            if (opt == 1) {
                read_line_input("\n\tEnter account to delete: ", buf, sizeof(buf));
                int acc = atoi(buf);
                for (int i = 0; i < count; ++i) {
                    if (custs[i].account == acc) { removed++; continue; }
                    newarr[newc++] = custs[i];
                }
            } else if (opt == 2) {
                read_line_input("\n\tEnter name to delete: ", buf, sizeof(buf));
                for (int i = 0; i < count; ++i) {
                    if (strcasecmp(custs[i].name, buf) == 0) { removed++; continue; }
                    newarr[newc++] = custs[i];
                }
            } else if (opt == 3) {
                read_line_input("\n\tEnter aadhaar to delete: ", buf, sizeof(buf));
                for (int i = 0; i < count; ++i) {
                    if (strcmp(custs[i].aadhaar, buf) == 0) { removed++; continue; }
                    newarr[newc++] = custs[i];
                }
            } else {
                printf("\n\tInvalid option\n");
                free(newarr); free(custs);
                return;
            }
            if (removed == 0) printf("\n\tNo matching records found.\n");
            save_customers(newarr, newc);
            printf("\n\tDeleted %d records.\n", removed);
            free(newarr);
        }
        free(custs);
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void update_data() {
    printf("\n\t1. Employee\n\t2. Customer\n");
    char buf[128];
    read_line_input("\n\tEnter: ", buf, sizeof(buf));
    int ch = atoi(buf);
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(emps);
            return;
        }
        read_line_input("\n\tEnter Employee ID to update: ", buf, sizeof(buf));
        int id = atoi(buf);
        
        int found = 0;
        for (int i = 0; i < count; ++i) {
            if (emps[i].id == id) {
                found = 1;
                printf("\n\tFound:\n");
                print_employees(&emps[i], 1);
                read_line_input("\n\tUpdate: 1.Name 2.Salary 3.Designation 4.All: ", buf, sizeof(buf));
                int opt = atoi(buf);
                if (opt == 1) { 
                    char temp[MAX_NAME];
                    while (1) {
                        read_line_input("\n\tNew name: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid name - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(emps[i].name, temp);
                        break;
                    }
                }
                else if (opt == 2) { 
                    char temp[32];
                    while (1) {
                        read_line_input("\n\tNew salary: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_numeric(temp)) {
                            printf("\n\tInvalid salary - must contain only digits\n");
                            continue;
                        }
                        strcpy(emps[i].salary, temp);
                        break;
                    }
                }
                else if (opt == 3) { 
                    char temp[MAX_DESIGN];
                    while (1) {
                        read_line_input("\n\tNew designation: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid designation - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(emps[i].designation, temp);
                        break;
                    }
                }
                else if (opt == 4) {
                    char temp[MAX_NAME];
                    /* Update name with validation */
                    while (1) {
                        read_line_input("\n\tNew name: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid name - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(emps[i].name, temp);
                        break;
                    }
                    /* Update salary with validation */
                    while (1) {
                        read_line_input("\n\tNew salary: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_numeric(temp)) {
                            printf("\n\tInvalid salary - must contain only digits\n");
                            continue;
                        }
                        strcpy(emps[i].salary, temp);
                        break;
                    }
                    /* Update designation with validation */
                    while (1) {
                        read_line_input("\n\tNew designation: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid designation - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(emps[i].designation, temp);
                        break;
                    }
                } else { printf("\n\tInvalid option\n"); }
                break;
            }
        }
        if (!found) printf("\n\tEmployee not found\n");
        save_employees(emps, count);
        free(emps);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(custs);
            return;
        }
        read_line_input("\n\tEnter Customer Account to update: ", buf, sizeof(buf));
        int acc = atoi(buf);
        
        int found = 0;
        for (int i = 0; i < count; ++i) {
            if (custs[i].account == acc) {
                found = 1;
                printf("\n\tFound:\n");
                print_customers(&custs[i], 1);
                read_line_input("\n\tUpdate: 1.Name 2.aadhaar 3.Phone 4.Address 5.Balance 6.All: ", buf, sizeof(buf));
                int opt = atoi(buf);
                if (opt == 1) {
                    char temp[MAX_NAME];
                    while (1) {
                        read_line_input("\n\tNew name: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid name - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(custs[i].name, temp);
                        break;
                    }
                }
                else if (opt == 2) {
                    char temp[MAX_AAD];
                    while (1) {
                        read_line_input("\n\tNew aadhaar: ", temp, sizeof(temp));
                        if (strlen(temp) != 12 || !is_numeric(temp)) {
                            printf("\n\tInvalid aadhaar - must be exactly 12 digits\n");
                            continue;
                        }
                        strcpy(custs[i].aadhaar, temp);
                        break;
                    }
                }
                else if (opt == 3) {
                    char temp[MAX_PHONE];
                    while (1) {
                        read_line_input("\n\tNew phone: ", temp, sizeof(temp));
                        if (strlen(temp) != 10 || !is_numeric(temp)) {
                            printf("\n\tInvalid phone - must be exactly 10 digits\n");
                            continue;
                        }
                        strcpy(custs[i].phone, temp);
                        break;
                    }
                }
                else if (opt == 4) {
                    char temp[MAX_ADDR];
                    while (1) {
                        read_line_input("\n\tNew address: ", temp, sizeof(temp));
                        if (strlen(temp) == 0) {
                            printf("\n\tAddress cannot be empty\n");
                            continue;
                        }
                        strcpy(custs[i].address, temp);
                        break;
                    }
                }
                else if (opt == 5) {
                    char temp[64];
                    while (1) {
                        read_line_input("\n\tNew balance: ", temp, sizeof(temp));
                        if (!is_numeric(temp)) {
                            printf("\n\tInvalid balance - must contain only digits\n");
                            continue;
                        }
                        custs[i].balance = atol(temp);
                        break;
                    }
                } else if (opt == 6) {
                    char temp[MAX_ADDR];
                    /* Update name with validation */
                    while (1) {
                        read_line_input("\n\tNew name: ", temp, sizeof(temp));
                        if (strlen(temp) == 0 || !is_alphabetic(temp)) {
                            printf("\n\tInvalid name - must contain only letters and spaces\n");
                            continue;
                        }
                        strcpy(custs[i].name, temp);
                        break;
                    }
                    /* Update aadhaar with validation */
                    while (1) {
                        read_line_input("\n\tNew aadhaar: ", temp, sizeof(temp));
                        if (strlen(temp) != 12 || !is_numeric(temp)) {
                            printf("\n\tInvalid aadhaar - must be exactly 12 digits\n");
                            continue;
                        }
                        strcpy(custs[i].aadhaar, temp);
                        break;
                    }
                    /* Update phone with validation */
                    while (1) {
                        read_line_input("\n\tNew phone: ", temp, sizeof(temp));
                        if (strlen(temp) != 10 || !is_numeric(temp)) {
                            printf("\n\tInvalid phone - must be exactly 10 digits\n");
                            continue;
                        }
                        strcpy(custs[i].phone, temp);
                        break;
                    }
                    /* Update address with validation */
                    while (1) {
                        read_line_input("\n\tNew address: ", temp, sizeof(temp));
                        if (strlen(temp) == 0) {
                            printf("\n\tAddress cannot be empty\n");
                            continue;
                        }
                        strcpy(custs[i].address, temp);
                        break;
                    }
                } else {
                    printf("\n\tInvalid option\n");
                }
                break;
            }
        }
        if (!found) printf("\n\tCustomer not found\n");
        save_customers(custs, count);
        free(custs);
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void create_export() {
    printf("\n\t1. Create TXT for Employees\n\t2. Create TXT for Customers\n");
    char buf[128];
    read_line_input("\n\tEnter: ", buf, sizeof(buf));
    int ch = atoi(buf);
    
    if (ch == 1) {
        Employee *emps = NULL; int count = 0;
        load_employees(&emps, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(emps);
            return;
        }
        read_line_input("\n\tEnter file name (without ext): ", buf, sizeof(buf));
        if (strlen(buf) == 0) { printf("\n\tInvalid file name\n"); return; }
        char path[512];
        snprintf(path, sizeof(path), "%s.txt", buf);
        FILE *f = fopen(path, "w");
        if (!f) { printf("\n\tUnable to create file\n"); free(emps); return; }
        for (int i = 0; i < count; ++i) {
            fprintf(f, "EMPLOYEE ID : %d  EMPLOYEE NAME : %s  EMPLOYEE DESIGNATION : %s\n",
                    emps[i].id, emps[i].name, emps[i].designation);
        }
        fclose(f);
        free(emps);
        printf("\n\tCreated employee file at %s\n", path);
    } else if (ch == 2) {
        Customer *custs = NULL; int count = 0;
        load_customers(&custs, &count);
        if (count == 0) {
            printf("\n\tData file was empty\n");
            free(custs);
            return;
        }
        read_line_input("\n\tEnter file name (without ext): ", buf, sizeof(buf));
        if (strlen(buf) == 0) { printf("\n\tInvalid file name\n"); return; }
        char path[512];
        snprintf(path, sizeof(path), "%s.txt", buf);
        FILE *f = fopen(path, "w");
        if (!f) { printf("\n\tUnable to create file\n"); free(custs); return; }
        for (int i = 0; i < count; ++i) {
            fprintf(f, "ACCOUNT NUMBER : %d  CUSTOMER NAME : %s  BANK ACCOUNT BALANCE : %ld\n",
                    custs[i].account, custs[i].name, custs[i].balance);
        }
        fclose(f);
        free(custs);
        printf("\n\tCreated customer file at %s\n", path);
    } else {
        printf("\n\tInvalid choice\n");
    }
}

void withdraw_amount() {
    char buf[64];
    read_line_input("\n\tEnter account number: ", buf, sizeof(buf));
    int acc = atoi(buf);
    Customer *custs = NULL; int count = 0;
    load_customers(&custs, &count);
    if (count == 0) {
        printf("\n\tData file was empty\n");
        free(custs);
        return;
    }
    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (custs[i].account == acc) {
            found = 1;
            printf("\n\tAccount: %d  Name: %s\n", custs[i].account, custs[i].name);
            printf("\n\tAvailable balance: %ld\n", custs[i].balance);
            if (custs[i].balance <= 1000) { printf("\n\tNo available balance to withdraw (min balance 1000 required)\n"); break; }
            
            read_line_input("\n\tEnter amount to withdraw: ", buf, sizeof(buf));
            if (!is_numeric(buf)) {
                printf("\n\tInvalid amount.\n");
                break;
            }
            long amount = atol(buf);
            if (amount <= 0) {
                printf("\n\tInvalid amount.\n");
                break;
            }
            
            long remaining = custs[i].balance - amount;
            if (remaining < 1000) { 
                printf("\n\tWithdrawal denied.\n"); 
                break; 
            }
            
            read_line_input("\n\tConfirm withdraw (YES/NO): ", buf, sizeof(buf));
            if (strcasecmp(buf, "YES") == 0) {
                custs[i].balance = remaining;
                save_customers(custs, count);
                printf("\n\tWithdrawn. Remaining balance: %ld\n", custs[i].balance);
            } else {
                printf("\n\tCancelled.\n");
            }
            break;
        }
    }
    if (!found) printf("\n\tAccount not found\n");
    free(custs);
}

void deposit_amount() {
    char buf[64];
    read_line_input("\n\tEnter account number: ", buf, sizeof(buf));
    int acc = atoi(buf);
    Customer *custs = NULL; int count = 0;
    load_customers(&custs, &count);
    if (count == 0) {
        printf("\n\tData file was empty\n");
        free(custs);
        return;
    }
    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (custs[i].account == acc) {
            found = 1;
            printf("\n\tAccount: %d  Name: %s\n", custs[i].account, custs[i].name);
            printf("\n\tAvailable balance: %ld\n", custs[i].balance);
            printf("\n\tNote: deposit min 1000, max 50000\n");
            
            read_line_input("\n\tEnter amount to deposit: ", buf, sizeof(buf));
            if (!is_numeric(buf)) {
                printf("\n\tInvalid amount\n");
                break;
            }
            long amount = atol(buf);
            if (amount < 1000 || amount > 50000) { 
                printf("\n\tInvalid amount.\n"); 
                break; 
            }
            
            read_line_input("\n\tConfirm deposit (YES/NO): ", buf, sizeof(buf));
            if (strcasecmp(buf, "YES") == 0) {
                custs[i].balance += amount;
                save_customers(custs, count);
                printf("\n\tDeposited. New balance: %ld\n", custs[i].balance);
            } else {
                printf("\n\tCancelled.\n");
            }
            break;
        }
    }
    if (!found) printf("\n\tAccount not found\n");
    free(custs);
}

/* ============================================================================
   MAIN MENU
   ============================================================================ */

int main(void) {
    while (1) {
        printf("\n\t----------------------BANKING MANAGEMENT SYSTEM----------------------\n");
        printf("\n\t\t1.CREATE NEW\n\t\t2.SEARCH DATA\n\t\t3.DELETE DATA\n\t\t4.UPDATE DATA\n\t\t5.VIEW ALL DATA\n\t\t6.CREATE EXPORT FILE\n\t\t7.WITHDRAWAL AMOUNT\n\t\t8.DEPOSIT AMOUNT\n\t\t9.EXIT PROGRAM\n");
        char buf[16];
        printf("\n\t\tENTER YOUR CHOICE:  ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        int ch = atoi(buf);
        switch (ch) {
            case 1: create_new(); break;
            case 2: search_data(); break;
            case 3: delete_data(); break;
            case 4: update_data(); break;
            case 5: view_all(); break;
            case 6: create_export(); break;
            case 7: withdraw_amount(); break;
            case 8: deposit_amount(); break;
            case 9:
                printf("\n\t\tTHANKS FOR USING OUR APPLICATION\n");
                exit(0);
                break;
            default:
                printf("\n\t\tINVALID CHOICE ENTERED\n");
                break;
        }
        printf("\n\t----------------------------------------------------------------------\n");
    }
    return 0;
}