#ifndef __EDITOR_H
#define __EDITOR_H

#include "display.h"

#define EDITOR_FIELD_COUNT 4U

typedef struct {
  uint8_t min;
  uint8_t max;
  uint8_t editable;
} EditorDigitRestriction;

typedef struct {
  uint8_t digits[EDITOR_FIELD_COUNT];
  EditorDigitRestriction restrictions[EDITOR_FIELD_COUNT];
  uint8_t selectedField;
} Editor;

void Editor_Begin(
    Editor *editor, const uint8_t digits[EDITOR_FIELD_COUNT],
    const EditorDigitRestriction restrictions[EDITOR_FIELD_COUNT]);
void Editor_SetSelectedField(Editor *editor, uint8_t field);
void Editor_SelectNext(Editor *editor);
void Editor_Increment(Editor *editor);
void Editor_Decrement(Editor *editor);
uint8_t Editor_GetDigit(const Editor *editor, uint8_t field);
void Editor_SetDigit(Editor *editor, uint8_t field, uint8_t value);
DisplayColumnMask Editor_GetSelectedColumn(const Editor *editor);

#endif