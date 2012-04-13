/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2012 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "GTUtilsDialog.h"
#include "api/GTComboBox.h"
#include "api/GTWidget.h"
#include "api/GTLineEdit.h"
#include "api/GTMenu.h"
#include "api/GTMouseDriver.h"
#include "api/GTKeyboardDriver.h"
#include <U2Gui/MainWindow.h>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QMenu>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QDialogButtonBox>

#include <QDebug>

namespace U2 {

void GUIDialogWaiter::wait() {
    QWidget *widget = NULL;

    switch (type) {
        case Modal:
            widget = QApplication::activeModalWidget();
            break;

        case Popup:
            widget = QApplication::activePopupWidget();
            break;

        default:
            break;
    }

    if (!widget) {
        return;
    }

    if (r && !hadRun) {
        hadRun = true;
        r->run();
    }
}

#define GT_CLASS_NAME "GTUtilsDialog"

#define GT_METHOD_NAME "waitForDialog"
void GTUtilsDialog::waitForDialog(U2OpStatus &os, Runnable *r, GUIDialogWaiter::DialogType type, bool failOnNoDialog) {

    GUIDialogWaiter waiter(r, type);
    QTimer t;

    t.connect(&t, SIGNAL(timeout()), &waiter, SLOT(wait()));
    t.start(100);

    GTGlobals::sleep(1000);
    if (failOnNoDialog) {
        GT_CHECK(waiter.hadRun == true, "no dialog");
    }
}
#undef GT_METHOD_NAME

void GTUtilsDialog::preWaitForDialog(U2OpStatus &os, Runnable *r, GUIDialogWaiter::DialogType _type)
{
    GUIDialogWaiter *waiter = new GUIDialogWaiter(r, _type);
    QTimer *t = new QTimer;

    t->connect(t, SIGNAL(timeout()), waiter, SLOT(wait()));
    t->start(100);
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::CreateAnnotationDialogChecker"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::CreateAnnotationDialogChecker::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QAbstractButton *radioButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "existingObjectRB", dialog));
    GT_CHECK(radioButton != NULL, "Radiobutton is NULL");
    GT_CHECK(!radioButton->isEnabled() && !radioButton->isCheckable(), "radioButton is enabled or checkable");

    QComboBox *comboBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "existingObjectCombo", dialog));
    GT_CHECK(comboBox != NULL, "ComboBox is NULL");
    GT_CHECK(comboBox->count() == 0, "comboBox count is not 0");

    QAbstractButton *b = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "existingObjectButton", dialog));
    GT_CHECK(b != NULL, "Button is NULL");
    GT_CHECK(b->isEnabled() == false, "Button is enabled");

    QWidget *cancelButton = GTWidget::findWidget(os, "cancel_button", dialog);
    GT_CHECK(cancelButton != NULL, "Cancel button is NULL");
    GTWidget::click(os, cancelButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::CreateAnnotationDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::CreateAnnotationDialogFiller::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *groupNameLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "groupNameEdit", dialog));
    GT_CHECK(groupNameLineEdit != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, groupNameLineEdit, groupName);

    QLineEdit *annotationNameLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "annotationNameEdit", dialog));
    GT_CHECK(annotationNameLineEdit != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, annotationNameLineEdit, annotationName);
 
	QLineEdit *locationLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "locationEdit", dialog));
    GT_CHECK(locationLineEdit != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, locationLineEdit, location);

    QAbstractButton *createButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "create_button", dialog));
    GTWidget::click(os, createButton);

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogChecker"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportProjectDialogChecker::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *projectFileLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "projectFileEdit", dialog));
    GT_CHECK(projectFileLineEdit != NULL, "LineEdit is NULL");
    GT_CHECK(projectFileLineEdit->text() == projectName, "Project name is not " + projectName);

    QAbstractButton *cancelButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "CancelButton", dialog));
    GT_CHECK(cancelButton != NULL, "CancelButton is NULL");
    GTWidget::click(os, cancelButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportProjectDialogFiller::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *projectFolderLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "exportFolderEdit", dialog));
    GT_CHECK(projectFolderLineEdit != NULL, "LineEdit is NULL");
    GTLineEdit::setText(os, projectFolderLineEdit, projectFolder);

    QLineEdit *projectFileLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "projectFileEdit", dialog));
    GT_CHECK(projectFileLineEdit != NULL, "LineEdit is NULL");
    if (!projectName.isEmpty()) {
        GTLineEdit::setText(os, projectFileLineEdit, projectName);
    }

    QAbstractButton *okButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "OKButton", dialog));
    GT_CHECK(okButton != NULL, "CancelButton is NULL");
    GTWidget::click(os, okButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::MessageBoxDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::MessageBoxDialogFiller::run() {

    QWidget* activeModal = QApplication::activeModalWidget();
    QMessageBox *messageBox = qobject_cast<QMessageBox*>(activeModal);
    GT_CHECK(messageBox != NULL, "messageBox is NULL");

    QAbstractButton* button = messageBox->button(b);
    GT_CHECK(button != NULL, "There is no such button in messagebox");

    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::SaveProjectAsDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::SaveProjectAsDialogFiller::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *projectNameEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "projectNameEdit", dialog));
    GTLineEdit::setText(os, projectNameEdit, projectName);

    QLineEdit *projectFolderEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "projectFolderEdit", dialog));
    GTLineEdit::setText(os, projectFolderEdit, projectFolder);

    QLineEdit *projectFileEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "projectFileEdit", dialog));
    GTLineEdit::setText(os, projectFileEdit, projectFile);

    QAbstractButton *createButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "createButton", dialog));
    GTWidget::click(os, createButton);

    GTUtilsDialog::MessageBoxDialogFiller filler(os, QMessageBox::Yes);
    GTUtilsDialog::waitForDialog(os, &filler, GUIDialogWaiter::Modal, false); // MessageBox question appears only if there is already a file on a disk
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

void GTUtilsDialog::PopupChooser::run()
{
    GTGlobals::sleep(100);
    QMenu* activePopupMenu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    GTMenu::clickMenuItem(os, activePopupMenu, namePath, useMethod);
}

#define GT_CLASS_NAME "GTUtilsDialog::RemoteDBDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::RemoteDBDialogFiller::run() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *idLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "idLineEdit", dialog));
    GTLineEdit::setText(os, idLineEdit, resID);

    QLineEdit *saveFilenameLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "saveFilenameLineEdit", dialog));
    if(!saveDirPath.isEmpty()){
        GTLineEdit::setText(os, saveFilenameLineEdit, saveDirPath);
    }

    QComboBox *databasesBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "databasesBox", dialog));
    GTComboBox::setCurrentIndex(os, databasesBox, DBItemNum);

    if (pressCancel) {
        QAbstractButton *rejectButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "rejectButton", dialog));
        GTWidget::click(os, rejectButton);
    }
    else {
        QAbstractButton *acceptButton = qobject_cast<QAbstractButton*>(GTWidget::findWidget(os, "acceptButton", dialog));
        GTWidget::click(os, acceptButton);
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportToSequenceFormatFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportToSequenceFormatFiller::run()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(lineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, lineEdit, path + name);

    QRadioButton *button = dialog->findChild<QRadioButton*>(QString::fromUtf8("keepGapsRB"));
    GT_CHECK(button != NULL, "radio button not found");

    switch(useMethod) {
    case GTGlobals::UseMouse:
        GTMouseDriver::moveTo(os, button->mapToGlobal(button->rect().topLeft()));
        GTMouseDriver::click(os);
        break;
    case GTGlobals::UseKey:
        GTWidget::setFocus(os, button);
        GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
        break;
    }
    GTGlobals::sleep(100);

    QPushButton *okButton = dialog->findChild<QPushButton*>(QString::fromUtf8("okButton"));
    GT_CHECK(okButton != NULL, "OK button not found");
    GTWidget::click(os, okButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportSelectedRegionFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportSelectedRegionFiller::run()
{
	QWidget *dialog = QApplication::activeModalWidget();
	GT_CHECK(dialog != NULL, "dialog not found");

	QLineEdit *lineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "fileNameEdit", dialog));
	GT_CHECK(lineEdit != NULL, "File name line edit not found");
	GTLineEdit::setText(os, lineEdit, path + name);

	QPushButton *okButton = dialog->findChild<QPushButton*>(QString::fromUtf8("exportButton"));
	GT_CHECK(okButton != NULL, "Export button not found");
	GTWidget::click(os, okButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportSequenceAsAlignmentFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportSequenceAsAlignmentFiller::run()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>();
    GT_CHECK(lineEdit != NULL, "line edit not found");

    GTLineEdit::setText(os, lineEdit, path + name);

    QComboBox *comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != NULL, "ComboBox not found");

    int index = -1;
    for (int i = 0; i < comboBox->count(); i++ ) {
        if (comboBox->itemText(i) == comboBoxItems[format]) {
            index = i;
            break;
        }
    }

    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    GTComboBox::setCurrentIndex(os, comboBox, index);

    QCheckBox *checkButton = dialog->findChild<QCheckBox*>(QString::fromUtf8("addToProjectBox"));
    GT_CHECK(checkButton, "Check box not found");

    if ((addToProject && !checkButton->isChecked()) ||
            !addToProject && checkButton->isChecked()) {
        switch(useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(os, checkButton->mapToGlobal(checkButton->rect().topLeft()));
            GTMouseDriver::click(os);
            break;
        case GTGlobals::UseKey:
            GTWidget::setFocus(os, checkButton);
            GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
            break;
        }
    }

    QPushButton *exportButton = dialog->findChild<QPushButton*>(QString::fromUtf8("okButton"));
    GT_CHECK(exportButton != NULL, "Export button not found");

    GTWidget::click(os, exportButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::CopyToFileAsDialogFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::CopyToFileAsDialogFiller::run()
{
	QWidget *dialog = QApplication::activeModalWidget();
	GT_CHECK(dialog != NULL, "dialog not found");

	QPushButton *btSave = dialog->findChild<QPushButton*>(QString::fromUtf8("createButton"));
	GT_CHECK(btSave != NULL, "Save button not found");
	GTWidget::click(os, btSave);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::run()
{
    QWidget *dlg = QApplication::activeModalWidget();
    GT_CHECK(dlg != NULL, "dialog not found");

    dialog = dlg;

    setFileName();
    GTGlobals::sleep(200);
    setFormat();
    GTGlobals::sleep(200);
    checkAddToProject();
    GTGlobals::sleep(200);
    clickMergeRadioButton();
    GTGlobals::sleep(200);
    fillSpinBox();
    GTGlobals::sleep(200);

    QPushButton *exportButton = dialog->findChild<QPushButton*>(QString::fromUtf8("exportButton"));
    GT_CHECK(exportButton != NULL, "Export button not found");

    GTWidget::click(os, exportButton);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFileName"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::setFileName()
{
    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GT_CHECK(lineEdit != NULL, "line edit not found");

    GTLineEdit::setText(os, lineEdit, path);
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFormat"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::setFormat()
{
    QComboBox *comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != NULL, "ComboBox not found");

    int index = -1;
    for (int i = 0; i < comboBox->count(); i++ ) {
        if (comboBox->itemText(i) == comboBoxItems[format]) {
            index = i;
            break;
        }
    }

    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

    if (comboBox->currentIndex() != index){
        GTComboBox::setCurrentIndex(os, comboBox, index);
    }
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAddToProject"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::checkAddToProject()
{
    QCheckBox *checkButton = dialog->findChild<QCheckBox*>(QString::fromUtf8("addToProjectBox"));
    GT_CHECK(checkButton != NULL, "Check box not found");

    if ((addToProject && !checkButton->isChecked()) ||
         !addToProject && checkButton->isChecked()) {
        QPoint checkPos = checkButton->mapToGlobal(checkButton->rect().topLeft());

        checkPos = QPoint(checkPos.x() + 10, checkPos.y() + 10); // +10 for moved to clickable area
        switch(useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(os, checkPos);
            GTMouseDriver::click(os);
            break;
        case GTGlobals::UseKey:
            GTWidget::setFocus(os, checkButton);
            GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
            break;
        }
    }
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickMergeRadioButton"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::clickMergeRadioButton()
{
    QRadioButton *mergeButton =  dialog->findChild<QRadioButton*>(mergeRadioButtons[options]);
    GT_CHECK(mergeButton != NULL, "Radio button " + mergeRadioButtons[options] + " not found");

    if (mergeButton->isEnabled()){
        QPoint radioPos = mergeButton->mapToGlobal(mergeButton->rect().topLeft());
        radioPos = QPoint(radioPos.x() + 10, radioPos.y() + 10); // +10 for moved to clickable area

        switch(useMethod) {
        case GTGlobals::UseMouse:
            GTMouseDriver::moveTo(os, radioPos);
            GTMouseDriver::click(os);
            break;
        case GTGlobals::UseKey:
            GTWidget::setFocus(os, mergeButton);
            GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
            break;
        }
    }
}

#undef GT_METHOD_NAME

#define GT_METHOD_NAME "fillSpinBox"
void GTUtilsDialog::ExportSequenceOfSelectedAnnotationsFiller::fillSpinBox()
{
    QSpinBox *mergeSpinBox = dialog->findChild<QSpinBox*>("mergeSpinBox");
    GT_CHECK(mergeSpinBox != NULL, "SpinBox not found");

    QPoint arrowPos;
    QRect spinBoxRect;
    int key;

    if (mergeSpinBox->value() != gapLength) {
        switch(useMethod) {
        case GTGlobals::UseMouse:
            spinBoxRect = mergeSpinBox->rect();
            if (gapLength > mergeSpinBox->value()) {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() / 4); // -5 it's needed that area under cursor was clickable
            } else {
                arrowPos = QPoint(spinBoxRect.right() - 5, spinBoxRect.height() * 3 / 4);
            }

            GTMouseDriver::moveTo(os, mergeSpinBox->mapToGlobal(arrowPos));
            while (mergeSpinBox->value() != gapLength) {
                GTMouseDriver::click(os);
                GTGlobals::sleep(100);
            }
            break;

        case GTGlobals::UseKey:
            if (gapLength > mergeSpinBox->value()) {
                key = GTKeyboardDriver::key["up"];
            } else {
                key = GTKeyboardDriver::key["down"];
            }

            GTWidget::setFocus(os, mergeSpinBox);
            while (mergeSpinBox->value() != gapLength) {
                GTKeyboardDriver::keyClick(os, key);
                GTGlobals::sleep(100);
            }
        }
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::exportAnnotationsFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::ExportAnnotationsFiller::run()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QLineEdit *lineEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GT_CHECK(lineEdit != NULL, "line edit not found");
    GTLineEdit::setText(os, lineEdit, exportToFile);

    QComboBox *comboBox = dialog->findChild<QComboBox*>();
    GT_CHECK(comboBox != NULL, "ComboBox not found");

    int index = -1;
    for (int i = 0; i < comboBox->count(); i++ ) {
        if (comboBox->itemText(i) == comboBoxItems[format]) {
            index = i;
            break;
        }
    }

    GT_CHECK(index != -1, QString("item \"%1\" in combobox not found").arg(comboBoxItems[format]));

	if (comboBox->currentIndex() != index){
		GTComboBox::setCurrentIndex(os, comboBox, index);
	}

    QCheckBox *checkButton = dialog->findChild<QCheckBox*>(QString::fromUtf8("exportSequenceCheck"));
    GT_CHECK(checkButton != NULL, "Check box not found");

    if ((saveSequencesUnderAnnotations && !checkButton->isChecked()) ||
            !saveSequencesUnderAnnotations && checkButton->isChecked()) {
		QPoint checkPos;
        switch(useMethod) {
        case GTGlobals::UseMouse:
            checkPos = QPoint(checkButton->rect().left() + 5, checkButton->rect().top() + 5);
            GTMouseDriver::moveTo(os, checkButton->mapToGlobal(checkPos));
            GTMouseDriver::click(os);
            break;
        case GTGlobals::UseKey:
            GTWidget::setFocus(os, checkButton);
            GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
            break;
        }
    }
    checkButton = dialog->findChild<QCheckBox*>(QString::fromUtf8("exportSequenceNameCheck"));
    GT_CHECK(checkButton != NULL, "Check box not found");

    if ((saveSequenceNames && !checkButton->isChecked()) ||
            !saveSequenceNames && checkButton->isChecked()) {
		QPoint checkPos;
        switch(useMethod) {
        case GTGlobals::UseMouse:
			checkPos = QPoint(checkButton->rect().left() + 5, checkButton->rect().top() + 5);
            GTMouseDriver::moveTo(os, checkButton->mapToGlobal(checkPos));
            GTMouseDriver::click(os);
            break;
        case GTGlobals::UseKey:
            GTWidget::setFocus(os, checkButton);
            GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["space"]);
            break;
        }
    }
	QDialogButtonBox* buttonBox = dialog->findChild<QDialogButtonBox*>("buttonBox");
    GT_CHECK(buttonBox != NULL, "buttonBox is NULL");

	QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    GT_CHECK(okButton != NULL, "okButton is NULL");

    GTWidget::click(os, okButton);

}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::exportSequenceOfSelectedAnnotationsFiller"
#define GT_METHOD_NAME "run"
void GTUtilsDialog::selectSequenceRegionDialogFiller::run()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if (selectAll) {
        QToolButton *min = dialog->findChild<QToolButton*>("minButton");
        QToolButton *max = dialog->findChild<QToolButton*>("maxButton");
        GT_CHECK(min != NULL, "Min button not found");
        GT_CHECK(max != NULL, "Max button not found");

        GTWidget::click(os, min);
        GTGlobals::sleep(500);
        GTWidget::click(os, max);
        GTGlobals::sleep(500);
    } else if (rangeType == Single) {
        GT_CHECK(min < max, "Value \"min\" greater or equals then \"max\"");

        QLineEdit *startEdit = dialog->findChild<QLineEdit*>("startEdit");
        QLineEdit *endEdit = dialog->findChild<QLineEdit*>("endEdit");
        GT_CHECK(startEdit != NULL, "QLineEdit \"startEdit\" not found");
        GT_CHECK(endEdit != NULL, "QLineEdit \"endEdit\" not found");

        GTLineEdit::setText(os, startEdit, QString::number(min));
        GTLineEdit::setText(os, endEdit, QString::number(max));
    } else {
        GT_CHECK(! multipleRange.isEmpty(), "Range is empty");

        QRadioButton *multipleButton = dialog->findChild<QRadioButton*>("miltipleButton");
        GT_CHECK(multipleButton != NULL, "RadioButton \"miltipleButton\" not found");

        QPoint buttonPos = multipleButton->mapToGlobal(multipleButton->rect().topLeft());
        buttonPos = QPoint(buttonPos.x() + 10, buttonPos.y() + 10); // for moved to clickable area

        GTMouseDriver::moveTo(os, buttonPos);
        GTMouseDriver::click(os);

        QLineEdit *regionEdit = dialog->findChild<QLineEdit*>("multipleRegionEdit");
        GT_CHECK(regionEdit != NULL, "QLineEdit \"multipleRegionEdit\" not foud");

        GTLineEdit::setText(os, regionEdit, multipleRange);
    }

    QPushButton *okButton = dialog->findChild<QPushButton*>("okButton");
    GT_CHECK(okButton != NULL, "OK button not found");
    GTWidget::click(os, okButton);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME


}
