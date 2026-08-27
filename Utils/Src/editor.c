#include "editor.h"

static uint8_t Editor_WrapIncrement(uint8_t value,
                                    EditorDigitRestriction restriction) {
  if (value >= restriction.max)
    return restriction.min;

  return value + 1;
}

static uint8_t Editor_WrapDecrement(uint8_t value,
                                    EditorDigitRestriction restriction) {
  if (value <= restriction.min)
    return restriction.max;

  return value - 1;
}

void Editor_Begin(
    Editor *editor, const uint8_t digits[EDITOR_FIELD_COUNT],
    const EditorDigitRestriction restrictions[EDITOR_FIELD_COUNT]) {

  for (uint8_t i = 0; i < EDITOR_FIELD_COUNT; i++) {
    editor->digits[i] = digits[i];
    editor->restrictions[i] = restrictions[i];
  }

  editor->selectedField = 0;
}

void Editor_SetSelectedField(Editor *editor, uint8_t field) {
  if (field < EDITOR_FIELD_COUNT)
    editor->selectedField = field;
}

void Editor_SelectNext(Editor *editor) {
  do {
    editor->selectedField++;

    if (editor->selectedField >= EDITOR_FIELD_COUNT)
      editor->selectedField = 0;

  } while (!editor->restrictions[editor->selectedField].editable);
}

void Editor_Increment(Editor *editor) {
  uint8_t field = editor->selectedField;
  if (!editor->restrictions[field].editable)
    return;
  editor->digits[field] =
      Editor_WrapIncrement(editor->digits[field], editor->restrictions[field]);
}

void Editor_Decrement(Editor *editor) {
  uint8_t field = editor->selectedField;
  if (!editor->restrictions[field].editable)
    return;
  editor->digits[field] =
      Editor_WrapDecrement(editor->digits[field], editor->restrictions[field]);
}

uint8_t Editor_GetDigit(const Editor *editor, uint8_t field) {
  return editor->digits[field];
}

void Editor_SetDigit(Editor *editor, uint8_t field, uint8_t value) {
  editor->digits[field] = value;
}

DisplayColumnMask Editor_GetSelectedColumn(const Editor *editor) {
  switch (editor->selectedField) {
  case 0:
    return DISPLAY_COLUMN_1;

  case 1:
    return DISPLAY_COLUMN_2;

  case 2:
    return DISPLAY_COLUMN_3;

  case 3:
    return DISPLAY_COLUMN_4;

  default:
    return DISPLAY_COLUMN_NONE;
  }
}