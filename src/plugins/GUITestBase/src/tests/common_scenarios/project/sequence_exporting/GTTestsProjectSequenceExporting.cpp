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

#include "GTTestsProjectSequenceExporting.h"
#include "api/GTGlobals.h"
#include <api/GTFileDialog.h>
#include <api/GTKeyboardDriver.h>
#include "api/GTMenu.h"
#include <api/GTMouseDriver.h>
#include "api/GTFile.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "api/GTTreeWidget.h"
#include "GTUtilsProject.h"
#include "GTUtilsDocument.h"
#include "GTUtilsLog.h"
#include "GTUtilsApp.h"
#include "GTUtilsToolTip.h"
#include "GTUtilsDialogRunnables.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsMdi.h"
#include "GTUtilsSequenceView.h"
#include <U2View/AnnotatedDNAViewFactory.h>
#include <U2View/MSAEditorFactory.h>
#include <U2Core/DocumentModel.h>
#include <U2View/ADVConstants.h>
#include <U2Core/AppContext.h>
#include <QFile>
#include <QPlainTextEdit>

namespace U2{

namespace GUITest_common_scenarios_project_sequence_exporting {

GUI_TEST_CLASS_DEFINITION(test_0001) {
// 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTUtilsProject::openFiles(os, testDir+"_common_data/scenarios/project/proj4.uprj");

// Expected state: 
//     1) Project view with document "1.gb" and "2.gb" is opened, both documents are unloaded    
    CHECK_SET_ERR(!GTUtilsDocument::getDocument(os, "1.gb")->isLoaded(), "1.gb is loaded");
    CHECK_SET_ERR(!GTUtilsDocument::getDocument(os, "2.gb")->isLoaded(), "2.gb is loaded");
//     2) UGENE window titled with text "proj4 UGENE"
    GTUtilsApp::checkUGENETitle(os, "proj4 UGENE");

// 2. Double click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep();

// Expected result: NC_001363 sequence has been opened in sequence view    
    GTUtilsDocument::checkDocument(os, "1.gb", AnnotatedDNAViewFactory::ID);

// 3. Select region 1..4 at sequence view. Right click to selected region open context menu. Use menu {Export->Export Selected Sequence region}
// Expected state: Export DNA Sequences To FASTA Format dialog open
// 4. Fill the next field in dialog:
// {Export to file:} _common_data/scenarios/sandbox/exp.fasta
// {Add created document to project} set checked

    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_selected_sequence_region", GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);
    Runnable *filler = new GTUtilsDialogRunnables::ExportSelectedRegionFiller(os, "_common_data/scenarios/sandbox/", "exp.fasta", GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, filler, GUIDialogWaiter::Modal);

    GTUtilsMdi::selectRandomRegion(os, "1 [s] NC_001363 sequence");
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep();

// Expected state: sequence view [1..4] has been opened, with sequence "AAAT"
    GTGlobals::sleep();
    GTUtilsDocument::checkDocument(os, "exp.fasta");
}

GUI_TEST_CLASS_DEFINITION(test_0002) {
// 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTUtilsProject::openFiles(os, testDir+"_common_data/scenarios/project/proj4.uprj");

// Expected state: 
//     1) Project view with document "1.gb" and "2.gb" is opened
    GTUtilsDocument::checkDocument(os, "1.gb");
    GTUtilsDocument::checkDocument(os, "2.gb");

//     2) UGENE window titled with text "proj4 UGENE"
    GTUtilsApp::checkUGENETitle(os, "proj4 UGENE");

// 2. Double click on "[a] Annotations" sequence object, in project view tree
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep();

//     Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, "1.gb", AnnotatedDNAViewFactory::ID);

// 3. Select annotation C. Use context menu item {Export->Export Sequence of Selected Annotations}
// Expected state: Export Sequence of selected annotations will open
// 
// 4. Fill the next field in dialog:
//     {Format } FASTA
//     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
//     {Add created document to project} set checked
// 5. Click Export button.

    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_sequence_of_selected_annotations", GTGlobals::UseMouse);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);

    Runnable *filler = new GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller(os, 
        "_common_data/scenarios/sandbox/exp.fasta",
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::Fasta,
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate
    );
    GTUtilsDialog::waitForDialog(os, filler, GUIDialogWaiter::Modal);

    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep();

// Expected state: sequence view C has been opened, with sequence "GAATAGAAAAG"
}

GUI_TEST_CLASS_DEFINITION(test_0003)
{
    const QString doc1("1.gb"), doc2("2.gb");

// 1. Use menu {File->Open}. Open project _common_data/scenario/project/proj4.uprj
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj4.uprj");
    GTGlobals::sleep();

// Expected state: 
//     1) Project view with document "1.gb" and "2.gb" is opened
    QTreeWidgetItem *item1 = GTUtilsProjectTreeView::findItem(os, doc1);
    QTreeWidgetItem *item2 = GTUtilsProjectTreeView::findItem(os, doc2);
    CHECK_SET_ERR(item1 != NULL && item2 != NULL, "Project view with document \"1.gb\" and \"2.gb\" is not opened");
//     2) UGENE window titled with text "proj4 UGENE"
    GTUtilsApp::checkUGENETitle(os, "proj4 UGENE");

// 2. Double click on "[a] Annotations" sequence object, in project view tree
    QPoint itemPos = GTUtilsProjectTreeView::getItemCenter(os, "Annotations");
    GTMouseDriver::moveTo(os, itemPos);
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(1000);

// Expected result: NC_001363 sequence has been opened in sequence view
    GTUtilsDocument::checkDocument(os, doc2, AnnotatedDNAViewFactory::ID);

// 3. Select joined annotation B. Use context menu item {Export->Export Sequence of Selected Annotations}
    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep();

// Expected state: Export Sequence of Selected Annotations
// 4. Fill the next field in dialog:
//     {Format } FASTA
//     {Export to file:} _common_data/scenarios/sandbox/exp.fasta
//     {Add created document to project} set checked
//     {Merge sequnces} set selected
//     {Gap length} 5
// 5. Click Export button.
    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_sequence_of_selected_annotations", GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);
    Runnable *filler = new GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller(os,
        testDir + "_common_data/scenarios/sandbox/exp.fasta",
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::Fasta,
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::Merge,
        5,
        true
    );
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep();

// Expected state: sequence view B part 1 of 3 has been opened, with sequence "ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG"
    GTUtilsSequenceView::checkSequence(os, "ACCCCACCCGTAGGTGGCAAGCTAGCTTAAG");
}

GUI_TEST_CLASS_DEFINITION(test_0004) {

    GTUtilsProject::openFiles(os, testDir+"_common_data/scenarios/project/proj4.uprj");
    GTUtilsDocument::checkDocument(os, "1.gb");
    GTUtilsDocument::checkDocument(os, "2.gb");
    GTUtilsApp::checkUGENETitle(os, "proj4 UGENE");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "Annotations"));
    GTMouseDriver::doubleClick(os);
    GTUtilsDocument::checkDocument(os, "1.gb", AnnotatedDNAViewFactory::ID);

    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "B_joined"));
    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << ADV_MENU_EXPORT << ACTION_EXPORT_ANNOTATIONS);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);

    Runnable *filler = new GTUtilsDialogRunnables::ExportAnnotationsFiller(os, testDir+"_common_data/scenarios/sandbox/1.csv", GTUtilsDialogRunnables::ExportAnnotationsFiller::csv);
    GTUtilsDialog::waitForDialog(os, filler, GUIDialogWaiter::Modal);
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(100);

    GTGlobals::sleep(1000);
    bool equals = GTFile::equals(os, testDir+"_common_data/scenarios/sandbox/1.csv", testDir+"_common_data/scenarios/project/test_0004.csv");
    CHECK_SET_ERR(equals == true, "Exported file differs from the test file");
}
GUI_TEST_CLASS_DEFINITION(test_0005) {
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/project/", "proj4.uprj");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "NC_001363 sequence"));
    GTMouseDriver::doubleClick(os);
    GTUtilsDocument::checkDocument(os, "1.gb", AnnotatedDNAViewFactory::ID);
    GTGlobals::sleep(100);

    QTreeWidgetItem* item = GTUtilsAnnotationsTreeView::findItem(os, "C");
    CHECK_SET_ERR(item != NULL, "AnnotationsTreeView is NULL");

    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "C"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep();

    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_annotations", GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);

    Runnable *filler = new GTUtilsDialogRunnables::ExportAnnotationsFiller(os,
        testDir+"_common_data/scenarios/sandbox/1.csv",
        GTUtilsDialogRunnables::ExportAnnotationsFiller::csv,
        true,
        false
    );
    GTUtilsDialog::waitForDialog(os, filler);
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep();

    QString expectedFileContent ("\"Group\",\"Name\",\"Start\",\"End\",\"Length\",\"Complementary\",\"Sequence\",\"qual1\",\"qual2\"\"C\",\"C\",\"80\",\"90\",\"11\",\"no\",\"GAATAGAAAAG\",\"val1\",\"val2\"");

    QFile file(testDir+"_common_data/scenarios/sandbox/1.csv");
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (! os.hasError()) {
            os.setError("Can't open file \"" + testDir + "_common_data/scenarios/sandbox/1.csv\"");
        }
    }
    QTextStream in(&file);
    QString fileContent, temp;
    
    while (! in.atEnd()) {
        in >> temp;
        fileContent += temp;
    }
    file.close();

    if (fileContent != expectedFileContent && !os.hasError() ) {
        os.setError("File is not expected file");
    }

}

GUI_TEST_CLASS_DEFINITION(test_0007) {

    Runnable *filler = new GTUtilsDialogRunnables::CreateDocumentFiller(os,
        "ACGTGTGTGTACGACAGACGACAGCAGACGACAGACAGACAGACAGCAAGAGAGAGAGAG",
        testDir + "_common_data/scenarios/sandbox/",
        GTUtilsDialogRunnables::CreateDocumentFiller::Genbank,
        "Sequence"
    );
    GTUtilsDialog::waitForDialog(os, filler);
    GTMenu::clickMenuItem(os, GTMenu::showMainMenu(os, MWMENU_FILE), "NewDocumentFromText", GTGlobals::UseKey);
    GTGlobals::sleep();

    GTGlobals::sleep();
    Runnable *filler2 = new GTUtilsDialogRunnables::CreateAnnotationDialogFiller(os, "<auto>", "misc_feature", "complement(1.. 20)");
    GTUtilsDialog::waitForDialog(os, filler2);
    GTKeyboardDriver::keyClick(os, 'n', GTKeyboardDriver::key["ctrl"]);
    GTGlobals::sleep();

    GTGlobals::sleep();


    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "misc_feature"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep();

    Runnable *popupChooser = new GTUtilsDialogRunnables::PopupChooser(os, QStringList() << "ADV_MENU_EXPORT" << "action_export_sequence_of_selected_annotations", GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, popupChooser, GUIDialogWaiter::Popup);
    Runnable *filler3 = new GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller(os,
        testDir + "_common_data/scenarios/sandbox/exp.gb",
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::Genbank,
        GTUtilsDialogRunnables::ExportSequenceOfSelectedAnnotationsFiller::SaveAsSeparate,
        0,
        true,
        true
    );
    GTUtilsDialog::waitForDialog(os, filler3);
    GTMouseDriver::moveTo(os, GTUtilsAnnotationsTreeView::getItemCenter(os, "misc_feature"));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep();

    GTGlobals::sleep();
}

}

}
