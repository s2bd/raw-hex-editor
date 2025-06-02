// Horrible Hexes (HH)
// @author Cooper Black (cooperblacks.github.io)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 16
#define MAX_UNDO 100

typedef struct{unsigned char data[BUFFER_SIZE];long offset;} UndoAction;
UndoAction undoStack[MAX_UNDO];
int undoCount=0;

// Index of all upcoming functions
void loadFile(const char *filename,unsigned char *buffer,long *size);
void saveFile(const char *filename,unsigned char *buffer,long size);
void displayHex(const unsigned char *buffer,long size,long offset);
void editByte(unsigned char *buffer,long offset,unsigned char newValue);
void pushUndo(long offset,unsigned char oldValue);
unsigned char popUndo(long *offset);

int main() {
    unsigned char *buffer;
    long size=0;
    char filename[256];
    char command[10];
    printf("Enter filename: ");scanf("%s", filename);
  
    buffer = (unsigned char *)malloc(1024 * 1024); // 1 MB buffer, 'cause why not?
    if(!buffer){perror("Failed to allocate memory!");return EXIT_FAILURE;}
    loadFile(filename, buffer, &size);
    while(1){
        displayHex(buffer, size, 0);
        printf("Enter command (edit <offset> <value>, undo, save, quit): ");scanf("%s", command);
        if(strcmp(command, "edit")==0){
            long offset;
            unsigned char value;
            scanf("%ld %hhx", &offset, &value);
            if(offset >= 0 && offset < size){
                pushUndo(offset, buffer[offset]); // Storing old values for undo function
                editByte(buffer, offset, value);
            }else{
                printf("Invalid offset!\n");
            }
        }else if(strcmp(command, "undo")==0){
            long offset;
            unsigned char oldValue=popUndo(&offset);
            if(undoCount>0){
                editByte(buffer, offset, oldValue);
                printf("Undid last change at offset %ld\n", offset);
            }else{printf("No actions to undo.\n");}
        }else if(strcmp(command, "save")==0){
            saveFile(filename, buffer, size);
        }else if(strcmp(command, "quit")==0){
            break;
        }else{printf("Unknown command!\n");}
    }
    free(buffer);
    return 0;
}

// Load the binary file into the buffer
void loadFile(const char *filename, unsigned char *buffer, long *size){
    FILE *file = fopen(filename, "rb");
    if(!file){perror("Error opening file!");exit(EXIT_FAILURE);}
    *size = fread(buffer, 1, 1024 * 1024, file); // Reading up to 1 MB
    fclose(file);
}
// Save the buffer to the binary file
void saveFile(const char *filename, unsigned char *buffer, long size){
    FILE *file = fopen(filename, "wb");
    if(!file){perror("Error saving file!");exit(EXIT_FAILURE);}
    fwrite(buffer, 1, size, file);
    fclose(file);
}

// Display all hexadecimal and ASCII representations of the buffer
void displayHex(const unsigned char *buffer, long size, long offset){
    printf("\nOffset    Hexadecimal                           ASCII\n");
    printf("--------------------------------------------------------\n");
    for(long i = 0; i < size; i += BUFFER_SIZE){
        printf("%08lX  ", i);
        for(int j=0; j<BUFFER_SIZE; j++){
            if(i+j<size){printf("%02X ", buffer[i + j]);
            }else{printf("   ");}
        }
        printf(" | ");
        for(int j=0; j<BUFFER_SIZE; j++){
            if(i+j<size){
                printf("%c", (buffer[i+j]>=32 && buffer[i+j]<=126) ? buffer[i+j] : '.');
            }else{printf(" ");}
        }
        printf("\n");
    }
}

// Edit a byte in the buffer
void editByte(unsigned char *buffer, long offset, unsigned char newValue){buffer[offset] = newValue;}

// Push
void pushUndo(long offset, unsigned char oldValue){
    if(undoCount < MAX_UNDO){
        undoStack[undoCount++] = (UndoAction){{oldValue}, offset};
    } else {
        printf("Undo stack is full!\n");
    }
}

// Pop
unsigned char popUndo(long *offset){
    if(undoCount>0){
        UndoAction action = undoStack[--undoCount];
        *offset = action.offset;
        return action.data[0];
    }
    return 0;
}

// That's all for now
