#include <stdio.h>
#include <stdlib.h>

typedef struct Block {
    int size;              // Tamanho do bloco
    struct Block* next;    // Ponteiro para o próximo bloco
    struct Block* prev;    // Ponteiro para o bloco anterior
} Block;

Block* allocatedListHead = NULL;    // Cabeça da lista de blocos alocados
Block* freeListHead = NULL;         // Cabeça da lista de blocos livres

void initialize() {
    freeListHead = (Block*)malloc(sizeof(Block));   // Aloca um bloco para a cabeça da lista de blocos livres
    freeListHead->size = 0;                         // Define o tamanho inicial como 0
    freeListHead->next = NULL;                      // Define o próximo bloco como nulo
    freeListHead->prev = NULL;                      // Define o bloco anterior como nulo
}

void* allocate(int size) {
    Block* curr = freeListHead->next;   // Ponteiro para percorrer a lista
    Block* prev = freeListHead;         // Ponteiro para o bloco anterior

    while (curr != NULL) {
        if (curr->size >= size) {
            if (curr->size >= size + sizeof(Block)) {
                // Se o bloco for maior que o necessário + tamanho da estrutura Block,
                // divide o bloco em dois, criando um novo bloco livre após o bloco alocado

                Block* newBlock = (Block*)((char*)curr + size + sizeof(Block));
                newBlock->size = curr->size - size - sizeof(Block);
                newBlock->next = curr->next;
                newBlock->prev = curr;

                curr->size = size;
                curr->next = newBlock;


                // Atualiza as referências do próximo e do bloco anterior
                if (newBlock->next != NULL) {
                    newBlock->next->prev = newBlock;
                }
            }
            else if (curr->next != NULL && curr->size + curr->next->size >= size + sizeof(Block)) {
                // Se o bloco não for grande o suficiente, mas o próximo bloco for e juntos forem suficientes,
                // combina os dois blocos em um único bloco alocado

                Block* nextBlock = curr->next;
                curr->size += sizeof(Block) + nextBlock->size;
                curr->next = nextBlock->next;

                // Atualiza as referências do próximo e do bloco anterior
                if (curr->next != NULL) {
                    curr->next->prev = curr;
                }
            }

            prev->next = curr->next;

            // Atualiza as referências do próximo e do bloco anterior
            if (curr->next != NULL) {
                curr->next->prev = prev;
            }

                        // Adiciona o bloco alocado à lista de blocos alocados
            if (allocatedListHead == NULL) {
                // Verifica se a lista de blocos alocados está vazia

                allocatedListHead = curr;
                // Se estiver vazia, o curr se torna a nova cabeça da lista de blocos alocados

                curr->next = NULL;
                curr->prev = NULL;
                // Configura os ponteiros next e prev do curr como nulos, já que ele é o único bloco na lista
            } else {
                // Se a lista de blocos alocados não estiver vazia

                curr->next = allocatedListHead;
                // Configura o ponteiro next do curr para apontar para o atual primeiro bloco alocado

                curr->prev = NULL;
                // Configura o ponteiro prev do curr como nulo, pois ele se tornará a nova cabeça da lista

                allocatedListHead->prev = curr;
                // Atualiza o ponteiro prev do antigo primeiro bloco alocado para apontar para o curr

                allocatedListHead = curr;
                // O curr se torna a nova cabeça da lista de blocos alocados
            }

            return curr;
        }

        prev = curr;
        curr = curr->next;
    }

    return NULL;
}


void deallocate(void* block) {
    Block* blk = (Block*)block;

    if (blk == NULL) {
        printf("Erro: O bloco fornecido é nulo.\n");
        return;
    }

    if (blk == allocatedListHead) {
        // Se o bloco a ser liberado é a cabeça da lista de blocos alocados

        allocatedListHead = blk->next; // Atualiza a cabeça da lista de blocos alocados para o próximo bloco (blk->next)

        if (blk->next != NULL) {
            blk->next->prev = NULL;
            // Atualiza o ponteiro prev do próximo bloco para nulo, removendo a referência ao bloco liberado
        }
    } else {
        // Se o bloco a ser liberado não é a cabeça da lista de blocos alocados

        if (blk->prev != NULL) {
            blk->prev->next = blk->next;
            // Atualiza o ponteiro next do bloco anterior para apontar para o próximo bloco do bloco liberado
        }

        if (blk->next != NULL) {
            blk->next->prev = blk->prev;
            // Atualiza o ponteiro prev do próximo bloco para apontar para o bloco anterior do bloco liberado
        }
    }

    blk->next = freeListHead->next;
    blk->prev = freeListHead;

    if (freeListHead->next != NULL) {
        freeListHead->next->prev = blk;
        // Atualiza o ponteiro prev do próximo bloco da lista de blocos livres para apontar para o bloco liberado
    }

    freeListHead->next = blk; // O bloco liberado se torna o novo primeiro bloco da lista de blocos livres
}


int main() {
    initialize();   // Inicializa a lista de blocos livres

    void* block1 = allocate(10);   // Aloca um bloco de memória de tamanho 10
    if (block1 != NULL)
        printf("Bloco 1 alocado com sucesso.\n");

    void* block2 = allocate(5);    // Aloca um bloco de memória de tamanho 5
    if (block2 != NULL)
        printf("Bloco 2 alocado com sucesso.\n");

    deallocate(block1);            // Libera o primeiro bloco alocado
    printf("Bloco 1 liberado.\n");

    void* block3 = allocate(8);    // Aloca um bloco de memória de tamanho 8
    if (block3 != NULL)
        printf("Bloco 3 alocado com sucesso.\n");

    deallocate(block2);            // Libera o segundo bloco alocado
    printf("Bloco 2 liberado.\n");

    return 0;
}