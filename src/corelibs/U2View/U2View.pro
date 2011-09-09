include (U2View.pri)

# Input
HEADERS += src/WebWindow.h \
           src/phyltree/BranchSettingsDialog.h \
           src/phyltree/ButtonSettingsDialog.h \
           src/phyltree/CreatePhyTreeDialogController.h \
           src/phyltree/CreatePhyTreeWidget.h \
           src/phyltree/TextSettingsDialog.h \
           src/phyltree/TreeSettingsDialog.h \
           src/util_dna_assembly/BuildIndexDialog.h \
           src/util_dna_assembly/ConvertAssemblyToSamDialog.h \
           src/util_dna_assembly/DnaAssemblyDialog.h \
           src/util_dna_assembly/DnaAssemblyGUIExtension.h \
           src/util_dna_assembly/DnaAssemblyUtils.h \
           src/util_find_dialog/FindDialog.h \
           src/util_msa_align/MSAAlignDialog.h \
           src/util_msa_align/MSAAlignGUIExtension.h \
           src/util_ov_annotated_dna/ADVAnnotationCreation.h \
           src/util_ov_annotated_dna/ADVClipboard.h \
           src/util_ov_annotated_dna/ADVConstants.h \
           src/util_ov_annotated_dna/ADVGraphModel.h \
           src/util_ov_annotated_dna/ADVSequenceObjectContext.h \
           src/util_ov_annotated_dna/ADVSequenceWidget.h \
           src/util_ov_annotated_dna/ADVSingleSequenceWidget.h \
           src/util_ov_annotated_dna/ADVSplitWidget.h \
           src/util_ov_annotated_dna/ADVSyncViewManager.h \
           src/util_ov_annotated_dna/ADVUtils.h \
           src/util_ov_annotated_dna/AnnotatedDNAView.h \
           src/util_ov_annotated_dna/AnnotatedDNAViewFactory.h \
           src/util_ov_annotated_dna/AnnotatedDNAViewState.h \
           src/util_ov_annotated_dna/AnnotatedDNAViewTasks.h \
           src/util_ov_annotated_dna/AnnotationsTreeView.h \
		   src/util_ov_annotated_dna/AnnotationsTreeViewL.h \
		   src/util_ov_annotated_dna/AnnotationsTreeModel.h \
           src/util_ov_annotated_dna/AutoAnnotationUtils.h \
           src/util_ov_annotated_dna/CreateRulerDialogController.h \
           src/util_ov_annotated_dna/DetView.h \
           src/util_ov_annotated_dna/EditAnnotationDialogController.h \
           src/util_ov_annotated_dna/GraphMenu.h \
           src/util_ov_annotated_dna/GSequenceGraphView.h \
           src/util_ov_annotated_dna/GSequenceGraphViewWithFactory.cpp \
           src/util_ov_annotated_dna/GSequenceLineView.h \
           src/util_ov_annotated_dna/GSequenceLineViewAnnotated.h \
           src/util_ov_annotated_dna/Overview.h \
           src/util_ov_annotated_dna/PanView.h \
           src/util_ov_annotated_dna/PanViewRows.h \
           src/util_ov_annotated_dna/WindowStepSelectorWidget.h \
           src/util_ov_indexview/CreateFileIndexDialog.h \
           src/util_ov_indexview/UIndexExportToNewFileDialogImpl.h \
           src/util_ov_indexview/UIndexSupport.h \
           src/util_ov_indexview/UIndexViewer.h \
           src/util_ov_indexview/UIndexViewerFactory.h \
           src/util_ov_indexview/UIndexViewerTasks.h \
           src/util_ov_indexview/UIndexViewHeaderItemWidgetImpl.h \
           src/util_ov_indexview/UIndexViewWidgetImpl.h \
           src/util_ov_indexview/UIndexViewWidgetKey.h \
           src/util_ov_msaedit/AlignmentLogo.h \
           src/util_ov_msaedit/ConsensusSelectorDialogController.h \
           src/util_ov_msaedit/CreateSubalignimentDialogController.h \
           src/util_ov_msaedit/DeleteGapsDialog.h \
           src/util_ov_msaedit/MSAColorScheme.h \
           src/util_ov_msaedit/MSAEditor.h \
           src/util_ov_msaedit/MSAEditorBaseOffsetsCache.h \
           src/util_ov_msaedit/MSAEditorConsensusArea.h \
           src/util_ov_msaedit/MSAEditorConsensusCache.h \
           src/util_ov_msaedit/MSAEditorFactory.h \
           src/util_ov_msaedit/MSAEditorNameList.h \
           src/util_ov_msaedit/MSAEditorOffsetsView.h \
           src/util_ov_msaedit/MSAEditorSequenceArea.h \
           src/util_ov_msaedit/MSAEditorState.h \
           src/util_ov_msaedit/MSAEditorStatusBar.h \
           src/util_ov_msaedit/MSAEditorTasks.h \
           src/util_ov_msaedit/MSAEditorUndoFramework.h \
           src/util_ov_msaedit/MSACollapsibleModel.h \
           src/util_ov_phyltree/CreateBranchesTask.h \
           src/util_ov_phyltree/CreateCircularBranchesTask.h \
           src/util_ov_phyltree/CreateRectangularBranchesTask.h \
           src/util_ov_phyltree/CreateUnrootedBranchesTask.h \
           src/util_ov_phyltree/GraphicsBranchItem.h \
           src/util_ov_phyltree/GraphicsButtonItem.h \
           src/util_ov_phyltree/GraphicsCircularBranchItem.h \
           src/util_ov_phyltree/GraphicsRectangularBranchItem.h \
           src/util_ov_phyltree/GraphicsUnrootedBranchItem.h \
           src/util_ov_phyltree/TreeViewer.h \
           src/util_ov_phyltree/TreeViewerFactory.h \
           src/util_ov_phyltree/TreeViewerState.h \
           src/util_ov_phyltree/TreeViewerTasks.h \
           src/util_ov_phyltree/TreeViewerUtils.h \
           src/util_ov_textview/SimpleTextObjectView.h \
           src/util_ov_textview/SimpleTextObjectViewTasks.h \
           src/util_sec_struct_predict/SecStructDialog.h \
           src/util_sec_struct_predict/SecStructPredictUtils.h \
           src/util_smith_waterman/SmithWatermanDialog.h \
           src/util_smith_waterman/SmithWatermanDialogImpl.h \
           src/util_smith_waterman/SubstMatrixDialog.h
FORMS += src/phyltree/ui/BranchSettingsDialog.ui \
         src/phyltree/ui/ButtonSettingsDialog.ui \
         src/phyltree/ui/CreatePhyTreeDialog.ui \
         src/phyltree/ui/TextSettingsDialog.ui \
         src/phyltree/ui/TreeSettingsDialog.ui \
         src/util_dna_assembly/ui/AssemblyToRefDialog.ui \
         src/util_dna_assembly/ui/AssemblyToSamDialog.ui \
         src/util_dna_assembly/ui/BuildIndexFromRefDialog.ui \
         src/util_find_dialog/ui/FindDialogUI.ui \
         src/util_msa_align/ui/PerformAlignmentDialog.ui \
         src/util_ov_annotated_dna/ui/CreateRulerDialog.ui \
         src/util_ov_annotated_dna/ui/EditAnnotationDialog.ui \
         src/util_ov_indexview/ui/CreateFileIndexDialog.ui \
         src/util_ov_indexview/ui/UIndexExportToNewFileDialog.ui \
         src/util_ov_indexview/ui/UIndexViewHeaderItemWidget.ui \
         src/util_ov_indexview/ui/UIndexViewWidget.ui \
         src/util_ov_msaedit/ui/ConsensusSelectorDialog.ui \
         src/util_ov_msaedit/ui/CreateSubalignimentDialog.ui \
         src/util_ov_msaedit/ui/DeleteGapsDialog.ui \
         src/util_sec_struct_predict/ui/SecStructDialog.ui \
         src/util_smith_waterman/ui/SmithWatermanDialogBase.ui \
         src/util_smith_waterman/ui/SubstMatrixDialogBase.ui
SOURCES += src/WebWindow.cpp \
           src/phyltree/BranchSettingsDialog.cpp \
           src/phyltree/ButtonSettingsDialog.cpp \
           src/phyltree/CreatePhyTreeDialogController.cpp \
           src/phyltree/CreatePhyTreeWidget.cpp \
           src/phyltree/TextSettingsDialog.cpp \
           src/phyltree/TreeSettingsDialog.cpp \
           src/util_dna_assembly/BuildIndexDialog.cpp \
           src/util_dna_assembly/ConvertAssemblyToSamDialog.cpp \
           src/util_dna_assembly/DnaAssemblyDialog.cpp \
           src/util_dna_assembly/DnaAssemblyUtils.cpp \
           src/util_find_dialog/FindDialog.cpp \
           src/util_msa_align/MSAAlignDialog.cpp \
           src/util_ov_annotated_dna/ADVAnnotationCreation.cpp \
           src/util_ov_annotated_dna/ADVClipboard.cpp \
           src/util_ov_annotated_dna/ADVGraphModel.cpp \
           src/util_ov_annotated_dna/ADVSequenceObjectContext.cpp \
           src/util_ov_annotated_dna/ADVSequenceWidget.cpp \
           src/util_ov_annotated_dna/ADVSingleSequenceWidget.cpp \
           src/util_ov_annotated_dna/ADVSplitWidget.cpp \
           src/util_ov_annotated_dna/ADVSyncViewManager.cpp \
           src/util_ov_annotated_dna/ADVUtils.cpp \
           src/util_ov_annotated_dna/AnnotatedDNAView.cpp \
           src/util_ov_annotated_dna/AnnotatedDNAViewFactory.cpp \
           src/util_ov_annotated_dna/AnnotatedDNAViewState.cpp \
           src/util_ov_annotated_dna/AnnotatedDNAViewTasks.cpp \
           src/util_ov_annotated_dna/AnnotationsTreeView.cpp \
		   src/util_ov_annotated_dna/AnnotationsTreeViewL.cpp \
		   src/util_ov_annotated_dna/AnnotationsTreeModel.cpp \
           src/util_ov_annotated_dna/AutoAnnotationUtils.cpp \
           src/util_ov_annotated_dna/CreateRulerDialogController.cpp \
           src/util_ov_annotated_dna/DetView.cpp \
           src/util_ov_annotated_dna/EditAnnotationDialogController.cpp \
           src/util_ov_annotated_dna/GraphMenu.cpp \
           src/util_ov_annotated_dna/GSequenceGraphView.cpp \
           src/util_ov_annotated_dna/GSequenceGraphViewWithFactory.cpp \
           src/util_ov_annotated_dna/GSequenceLineView.cpp \
           src/util_ov_annotated_dna/GSequenceLineViewAnnotated.cpp \
           src/util_ov_annotated_dna/Overview.cpp \
           src/util_ov_annotated_dna/PanView.cpp \
           src/util_ov_annotated_dna/PanViewRows.cpp \
           src/util_ov_annotated_dna/WindowStepSelectorWidget.cpp \
           src/util_ov_indexview/UIndexExportToNewFileDialogImpl.cpp \
           src/util_ov_indexview/UIndexSupport.cpp \
           src/util_ov_indexview/UIndexViewer.cpp \
           src/util_ov_indexview/UIndexViewerFactory.cpp \
           src/util_ov_indexview/UIndexViewerTasks.cpp \
           src/util_ov_indexview/UIndexViewHeaderItemWidgetImpl.cpp \
           src/util_ov_indexview/UIndexViewWidgetImpl.cpp \
           src/util_ov_indexview/UIndexViewWidgetKey.cpp \
           src/util_ov_msaedit/AlignmentLogo.cpp \
           src/util_ov_msaedit/ConsensusSelectorDialogController.cpp \
           src/util_ov_msaedit/CreateSubalignimentDialogController.cpp \
           src/util_ov_msaedit/DeleteGapsDialog.cpp \
           src/util_ov_msaedit/MSAColorScheme.cpp \
           src/util_ov_msaedit/MSAEditor.cpp \
           src/util_ov_msaedit/MSAEditorBaseOffsetsCache.cpp \
           src/util_ov_msaedit/MSAEditorConsensusArea.cpp \
           src/util_ov_msaedit/MSAEditorConsensusCache.cpp \
           src/util_ov_msaedit/MSAEditorFactory.cpp \
           src/util_ov_msaedit/MSAEditorNameList.cpp \
           src/util_ov_msaedit/MSAEditorOffsetsView.cpp \
           src/util_ov_msaedit/MSAEditorSequenceArea.cpp \
           src/util_ov_msaedit/MSAEditorState.cpp \
           src/util_ov_msaedit/MSAEditorStatusBar.cpp \
           src/util_ov_msaedit/MSAEditorTasks.cpp \
           src/util_ov_msaedit/MSAEditorUndoFramework.cpp \
           src/util_ov_msaedit/MSACollapsibleModel.cpp \
           src/util_ov_phyltree/CreateCircularBranchesTask.cpp \
           src/util_ov_phyltree/CreateRectangularBranchesTask.cpp \
           src/util_ov_phyltree/CreateUnrootedBranchesTask.cpp \
           src/util_ov_phyltree/GraphicsBranchItem.cpp \
           src/util_ov_phyltree/GraphicsButtonItem.cpp \
           src/util_ov_phyltree/GraphicsCircularBranchItem.cpp \
           src/util_ov_phyltree/GraphicsRectangularBranchItem.cpp \
           src/util_ov_phyltree/GraphicsUnrootedBranchItem.cpp \
           src/util_ov_phyltree/TreeViewer.cpp \
           src/util_ov_phyltree/TreeViewerFactory.cpp \
           src/util_ov_phyltree/TreeViewerState.cpp \
           src/util_ov_phyltree/TreeViewerTasks.cpp \
           src/util_ov_phyltree/TreeViewerUtils.cpp \
           src/util_ov_textview/SimpleTextObjectView.cpp \
           src/util_ov_textview/SimpleTextObjectViewTasks.cpp \
           src/util_sec_struct_predict/SecStructDialog.cpp \
           src/util_sec_struct_predict/SecStructPredictUtils.cpp \
           src/util_smith_waterman/SmithWatermanDialog.cpp \
           src/util_smith_waterman/SubstMatrixDialog.cpp
TRANSLATIONS += transl/czech.ts transl/english.ts transl/russian.ts
