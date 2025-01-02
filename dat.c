#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat st = {0};

// Function to create a part (file)
int add_part(char *name) {
    FILE *fptr;
    char file_name[256];  // Buffer for file name

    // Create the file name by appending ".txt" to the name
    snprintf(file_name, sizeof(file_name), "%s.txt", name);

    // Open the file for writing
    fptr = fopen(file_name, "w");
    if (fptr == NULL) {
        perror("Error creating file");
        return 1;  // Return an error code
    }

    fclose(fptr);  // Close the file
    printf("Page '%s' created successfully!\n", file_name);
    return 0;
}

// Function to edit a part (file)
int edit_part(char *file_name) {
    printf("Editing file: %s\n", file_name);

    FILE *fptr = fopen(file_name, "a");  // Open file in append mode
    if (fptr == NULL) {
        perror("Error opening file");
        return 1;  // Return an error code
    }

    // Example content to append (you can modify this for real editing)
    fprintf(fptr, "Adding some example content...\n");

    fclose(fptr);  // Close the file
    printf("File '%s' edited successfully!\n", file_name);
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

    // Check if the directory exists
    if (stat(path, &st) == -1) {
        printf("No such directory. Please create it first.\n");
        return 1;
    }

    printf("Input the name for the wiki: \n");
    scanf("%s", name);

    // Construct paths for wiki directory and docs directory
    snprintf(wiki_dir, sizeof(wiki_dir), "%s/%s", path, name);
    snprintf(docs_dir, sizeof(docs_dir), "%s/docs", wiki_dir);

    // Create the wiki directory
    if (mkdir(wiki_dir, 0777) == -1) {
        perror("Error creating wiki directory");
        return 1;
    }

    // Create the docs directory
    if (mkdir(docs_dir, 0777) == -1) {
        perror("Error creating docs directory");
        return 1;
    }

    // Create the main wiki file
    char wiki_file[256];
    snprintf(wiki_file, sizeof(wiki_file), "%s/wiki.txt", wiki_dir);

    fptr = fopen(wiki_file, "w");
    if (fptr == NULL) {
        perror("Error creating main wiki file");
        return 1;
    }

    fprintf(fptr, "Welcome to your wiki: %s\n", name);
    printf("Enter the description for your new wiki: \n");
    scanf(" %[^\n]", description); // Read full line for description
    fprintf(fptr, "%s\n", description);

    fclose(fptr);

    // Append to global wikis file
    config = fopen("/var/dat/wikis.txt", "a");
    if (config == NULL) {
        perror("Error opening global wikis file");
        return 1;
    }
    fprintf(config, "%s::%s\n", name, wiki_dir); // Write wiki name and path
    fclose(config);

    printf("Wiki '%s' initialized successfully at '%s'!\n", name, path);
    return 0;
}


int initial() {
    FILE *fptr;
    if (stat("/var/dat", &st) == -1) {
        mkdir("/var/dat", 0777);  // Create /var/dat directory
        fptr = fopen("/var/dat/wikis.txt", "w");  // Create wikis.txt
        if (fptr) fclose(fptr);
        printf("Added the init files.\n");
    } else if (stat("/var/dat/wikis.txt", &st) == -1) {
        fptr = fopen("/var/dat/wikis.txt", "w");
        if (fptr) fclose(fptr);
        printf("Added the wiki files.\n");
    } else {
        printf("The files are already there.\n");
    }
    return 0;
}


int main(int argc, char *argv[]) {
    int opt;
    char *tValue = NULL;

    static struct option long_options[] = {
        {"init", no_argument, NULL, 'i'},      // Initialize global config
        {"newwiki", no_argument, NULL, 'n'},  // Create a new wiki
        {"help", no_argument, NULL, 'h'},     // Show help
        {"add", required_argument, NULL, 'a'}, // Add a new part
        {"edit", required_argument, NULL, 'e'}, // Edit a part
        {NULL, 0, NULL, 0}                    // End of the options list
    };

    while ((opt = getopt_long(argc, argv, "a:e:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                tValue = optarg;
                add_part(tValue);
                break;

            case 'e':
                tValue = optarg;
                edit_part(tValue);
                break;

            case 'h':
                printf("Here are your options:\n");
                printf(" '--help' - Show this help panel\n");
                printf(" '--init' - Initialize global configuration\n");
                printf(" '--newwiki' - Create a new wiki\n");
                printf(" '--add <name>' - Add a new page\n");
                printf(" '--edit <file_name>' - Edit an existing page\n");
                return 0;

            case 'n':
                init_config();
                break;

            case 'i':
                initial();
                break;

            default:
                fprintf(stderr, "Usage: %s [--add name] [--edit file_name] [--init] [--help]\n", argv[0]);
                return 1;
        }
    }

    return 0;
}
