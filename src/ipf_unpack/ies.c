/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "ies.h"
#include "dbg/dbg.h"

void ies_decrypt_string (char *string, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (string[i] != '\0') {
            string[i] ^= 0x01;
        }
    }
}

bool ies_read (uint8_t *ies, size_t size, IesCallback callback, void *userdata) 
{
    bool status = false;
    IesTable table = {0};
    IesHeader *header = (void *) ies;

    // Read and sort int/str columns
    IesColumn *columns = NULL;
    if (!(columns = calloc (header->colsCount, sizeof(IesColumn)))) {
        error ("Cannot allocate columns (Count = %d)", header->colsCount);
        goto cleanup;
    }

    // Read int/str columns
    IesColumn *curCol = (IesColumn *) &ies[header->fileSize - (header->resourceOffset + header->dataOffset)];
    IesColumn *intCols = &columns[0];
    IesColumn *strCols = &columns[header->intColsCount];
    for (int i = 0, intIdx = 0, strIdx = 0; i < header->colsCount; i++, curCol++)
    {
        ies_decrypt_string (curCol->name, sizeof(curCol->name));
        ies_decrypt_string (curCol->name2, sizeof(curCol->name2));

        switch (curCol->type) {
            case 0:
                memcpy (&intCols[intIdx++], curCol, sizeof(IesColumn));
            break;

            case 1:
            case 2:
                memcpy (&strCols[strIdx++], curCol, sizeof(IesColumn));
            break;
        }
    }

    // Sort int/str columns
    int sortColumns (const void *_col1, const void *_col2) {
        IesColumn *col1 = (void *) _col1;
        IesColumn *col2 = (void *) _col2;
        return col1->position - col2->position;
    }
    qsort (intCols, header->intColsCount, sizeof(IesColumn), sortColumns);
    qsort (strCols, header->strColsCount, sizeof(IesColumn), sortColumns);

    // Allocate IesTable
    size_t rowsCount = 0;
    table.rows = calloc (header->rowsCount, sizeof(IesRow));
    table.header = header;
    table.columns = columns;

    uint8_t *cursorRows = &ies[header->fileSize - header->resourceOffset];
    for (int rowId = 0; rowId < header->rowsCount; rowId++) 
    {
        #pragma pack(push, 1)
        IesCellOpt(0) *cellOpt = (void *) cursorRows;
        #pragma pack(pop)

        ies_decrypt_string (cellOpt->name, cellOpt->size);
        cursorRows += sizeof(*cellOpt) + cellOpt->size;

        curCol = columns;
        
        // Allocate IesRow
        IesRow *curRow = &table.rows[rowsCount++];
        curRow->cellsCount = 0;
        curRow->cells = calloc(header->colsCount, sizeof(IesCell(0, 0) *));

        for (int colId = 0; colId < header->colsCount; colId++, curCol++) 
        {
            switch (curCol->type) {
                case 0: {
                    // float
                    #pragma pack(push, 1)
                    struct FloatCol {
                        float value;
                    } *rowFlt = (void *) cursorRows;
                    #pragma pack(pop)

                    // Allocate the IesCell
                    IesCell (cellOpt->size, 0) *cell = calloc (1, sizeof (IesCell (cellOpt->size, 0)));
                    memcpy (&cell->opt, cellOpt, sizeof(cell->opt));
                    memcpy (&cell->flt, rowFlt, sizeof(cell->flt));
                    cell->col = curCol;
                    cell->optSize = cellOpt->size;
                    cell->strSize = 0;
                    curRow->cells[curRow->cellsCount++] = (void *) cell;

                    cursorRows += sizeof(*rowFlt);
                } break;

                case 1:
                case 2: {
                    // string
                    #pragma pack(push, 1)
                    struct StringCol {
                        uint16_t size;
                        char string[0];
                    } *cellStr = (void *) cursorRows;
                    #pragma pack(pop)
                    ies_decrypt_string (cellStr->string, cellStr->size);

                    // Allocate the IesCell
                    IesCell (cellOpt->size, cellStr->size) *cell = calloc (1, sizeof (IesCell (cellOpt->size, cellStr->size)));
                    memcpy (&cell->opt, cellOpt, sizeof(cell->opt));
                    memcpy (&cell->str, cellStr, sizeof(cell->str));
                    cell->col = curCol;
                    cell->optSize = cellOpt->size;
                    cell->strSize = cellStr->size;
                    cell->str.value[cellStr->size] = '\0';
                    curRow->cells[curRow->cellsCount++] = (void *) cell;

                    cursorRows += sizeof(*cellStr) + cellStr->size;
                } break;

                default : error ("Unknown type : %x", curCol->type); goto cleanup; break;
            }
        }

        cursorRows += header->strColsCount;
    }

    if (!(callback (&table, userdata))) {
        error ("Callback failed for column '%s'.", curCol->name);
        goto cleanup;
    }

    status = true;
cleanup:
    for (int rowId = 0; rowId < header->rowsCount; rowId++) {
        IesRow *curRow = &table.rows[rowId];
        for (int cellId = 0; cellId < curRow->cellsCount; cellId++) {
            IesCell(0, 0) *cell = (void *) curRow->cells[cellId];
            free (cell);
        }
        free (curRow->cells);
    }
    free (table.rows);
    free (columns);
    return status;
}