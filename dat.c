#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat st = {0};

// Function to get the directory of a specific wiki from wikis.txt
int get_wiki_dir(const char *wiki_name, char *wiki_dir) {
    FILE *file = fopen("/var/dat/wikis.txt", "r");
    if (file == NULL) {
        perror("Error opening wikis.txt");
        return 1;
    }

    char line[1024];
    const char delim[] = "::";
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove the newline character
        char *token = strtok(line, delim);

        // If the wiki name matches, return the directory path
        if (strcmp(token, wiki_name) == 0) {
            token = strtok(NULL, delim);
            strcpy(wiki_dir, token);  // Copy the wiki directory path
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    printf("Wiki '%s' not found.\n", wiki_name);
    return 1;
}

// Function to create a part (file)
// Function to create a part (file) in a specific wiki
int add_part(char *page_name, char *wiki_name) {
    char wiki_dir[256];
    if (get_wiki_dir(wiki_name, wiki_dir) != 0) {
        return 1;  // Wiki not found
    }

    // Build the full file path for the page
    char file_name[512];
    snprintf(file_name, sizeof(file_name), "%s/docs/%s.txt", wiki_dir, page_name);

    // Check if the directory exists
    if (stat(wiki_dir, &st) == -1) {
        printf("Wiki directory '%s' does not exist.\n", wiki_dir);
        return 1;
    }

    // Create the file
    FILE *fptr = fopen(file_name, "w");
    if (fptr == NULL) {
        perror("Error creating file");
        return 1;
    }
    fclose(fptr);

    printf("Page '%s' created successfully in wiki '%s'!\n", file_name, wiki_name);
    return 0;
}


// Function to edit a part (file)
int edit_part(char *wiki_name, char *file_name) {
    char wiki_dir[256];
    if (get_wiki_dir(wiki_name, wiki_dir) != 0) {
        return 1;  // Wiki not found
    }

    // Build the full file path for the part
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/docs/%s.txt", wiki_dir, file_name);

    // Open the file for appending
    FILE *fptr = fopen(file_path, "a");
    if (fptr == NULL) {
        perror("Error opening file");
        return 1;
    }
    fprintf(fptr, "Adding some example content...\n");
    fclose(fptr);

    printf("File '%s' edited successfully in wiki '%s'!\n", file_path, wiki_name);
    return 0;
}

int print_wikis(){
    FILE *file = fopen("/var/dat/wikis.txt", "r"); // Open the file in read mode
        if (file == NULL) {
            perror("Error opening file");
            return 1;
        }

        char line[1024]; // Buffer to store each line from the file
        const char delim[] = "::"; // Delimiter

        // Read the file line by line
        while (fgets(line, sizeof(line), file)) {
            // Remove the newline character, if present
            line[strcspn(line, "\n")] = 0;
            // Tokenize the line using `::` as the delimiter
            char *token = strtok(line, delim);
            while (token != NULL) {
                printf("Token: %s\n", token); // Process each token
                token = strtok(NULL, delim); // Get the next token
            }
        }
        fclose(file); // Close the file
        return 0;}
// Function to handle initialization
int init_config() {
    FILE *fptr, *config;
    char path[128], name[127], wiki_dir[256], docs_dir[256], description[512];

    printf("Choose a directory for your wiki: \n");
    scanf("%s", path);

    if (stat(path, &st) == -1) {
        printf("No such directory. Please create it first.\n");
        return 1;
    }

    printf("Input the name for the wiki: \n");
    scanf("%s", name);

    snprintf(wiki_dir, sizeof(wiki_dir), "%s/%s", path, name);
    snprintf(docs_dir, sizeof(docs_dir), "%s/docs", wiki_dir);

    // Create directories
    mkdir(wiki_dir, 0777);
    mkdir(docs_dir, 0777);

    char wiki_file[256];
    snprintf(wiki_file, sizeof(wiki_file), "%s/wiki.txt", wiki_dir);

    fptr = fopen(wiki_file, "w");
    if (fptr == NULL) {
        perror("Error creating wiki file");
        return 1;
    }

    fprintf(fptr, "Welcome to your wiki: %s\n", name);
    printf("Enter the description for your new wiki: \n");
    scanf(" %[^\n]", description);
    fprintf(fptr, "%s\n", description);
    fclose(fptr);

    config = fopen("/var/dat/wikis.txt", "a");
    if (config == NULL) {
        perror("Error opening wikis.txt");
        return 1;
    }
    fprintf(config, "%s::%s\n", name, wiki_dir);
    fclose(config);

    printf("Wiki '%s' initialized successfully at '%s'.\n", name, path);
    return 0;
}
int initial() {
    FILE *fptr;
    if (stat("/var/dat", &st) == -1) {
        mkdir("/var/dat", 0777);  // Create /var/dat directory
        fptr = fopen("/var/dat/wikis.txt", "w");  // Create wikis.txt
        if (fptr) fclose(fptr);
        printf("Initialization: Created '/var/dat' and 'wikis.txt'.\n");
    } else if (stat("/var/dat/wikis.txt", &st) == -1) {
        fptr = fopen("/var/dat/wikis.txt", "w");
        if (fptr) fclose(fptr);
        printf("Initialization: Added 'wikis.txt'.\n");
    } else {
        printf("Initialization: All files and directories already exist.\n");
    }
    return 0;
}
int main(int argc, char *argv[]) {
    int opt;
    char *page_name = NULL;
    char *wiki_name = NULL;

    static struct option long_options[] = {
        {"newwiki", no_argument, NULL, 'n'},  // Create a new wiki
        {"help", no_argument, NULL, 'h'},     // Show help
        {"add", required_argument, NULL, 'a'}, // Add a new part (page)
        {"edit", required_argument, NULL, 'e'}, // Edit a part
        {"wiki", required_argument, NULL, 'w'}, // Specify the wiki name
        {NULL, 0, NULL, 0}                    // End of the options list
    };

    while ((opt = getopt_long(argc, argv, "a:e:h:nw:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                page_name = optarg;  // Get the name of the page to add
                if (optind < argc) {
                    wiki_name = argv[optind];  // Get the wiki name
                    add_part(page_name, wiki_name);  // Add page in the specified wiki
                } else {
                    fprintf(stderr, "Error: Missing wiki name.\n");
                    return 1;
                }
                break;

            case 'e':
                page_name = optarg;  // Get the page name to edit
                if (optind < argc) {
                    wiki_name = argv[optind];  // Get the wiki name
                    edit_part(wiki_name, page_name);  // Edit page in the specified wiki
                } else {
                    fprintf(stderr, "Error: Missing wiki name.\n");
                    return 1;
                }
                break;

            case 'h':
                printf("Here are your options:\n");
                printf(" --help           Show this help panel\n");
                printf(" --newwiki        Create a new wiki\n");
                printf(" --add <PageName> <WikiName>  Add a new page\n");
                printf(" --edit <PageName> <WikiName> Edit an existing page\n");
                printf(" --wiki <name>    Specify the wiki for editing\n");
                return 0;

            case 'n':
                init_config();
                break;

            case 'w':
                wiki_name = optarg;  // Specify the wiki name for editing
                break;

            default:
                fprintf(stderr, "Usage: %s [--add PageName WikiName] [--edit PageName WikiName] [--newwiki] [--help] [--wiki name]\n", argv[0]);
                return 1;
        }
    }

    return 0;
}
