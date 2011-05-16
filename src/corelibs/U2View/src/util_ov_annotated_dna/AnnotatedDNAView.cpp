/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
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

#include "AnnotatedDNAView.h"
#include "AnnotatedDNAViewState.h"
#include "AnnotatedDNAViewFactory.h"
#include "AnnotatedDNAViewTasks.h"
#include "ADVUtils.h"

#include "AnnotationsTreeView.h"
#include "ADVAnnotationCreation.h"
#include "ADVClipboard.h"
#include "ADVConstants.h"
#include "ADVSequenceObjectContext.h"
#include "ADVSingleSequenceWidget.h"
#include "ADVSplitWidget.h"
#include "ADVSyncViewManager.h"
#include "AutoAnnotationUtils.h"

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Timer.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/AutoAnnotationsSupport.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GObjectRelationRoles.h>

#include <U2Core/SelectionUtils.h>
#include <U2Core/AnnotationSelection.h>
#include <U2Core/DNASequenceSelection.h>

#include <U2Core/AddPartToSequenceTask.h>
#include <U2Core/ReplacePartOfSequenceTask.h>

#include <U2View/FindDialog.h>//BUG:423: move to plugins!?
#include <U2View/SecStructPredictUtils.h>

#include <U2Gui/AnnotationSettingsDialogController.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/CreateObjectRelationDialogController.h>
#include <U2Misc/PositionSelector.h>
#include <U2Misc/DialogUtils.h>
#include <U2Gui/EditSequenceDialogController.h>
#include <U2Gui/RemovePartFromSequenceDialogController.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

#include <memory>

namespace U2 {

/* TRANSLATOR U2::AnnotatedDNAView */

AnnotatedDNAView::AnnotatedDNAView(const QString& viewName, const QList<DNASequenceObject*>& dnaObjects) 
: GObjectView(AnnotatedDNAViewFactory::ID, viewName)
{
    timerId = 0;

    annotationSelection = new AnnotationSelection(this);
    annotationGroupSelection = new AnnotationGroupSelection(this);
    
    clipb = NULL;
    
    mainSplitter = NULL;
    scrollArea = NULL;
    posSelector = NULL;
    posSelectorWidgetAction = NULL;
    annotationsView = NULL;
    focusedWidget = NULL;

    createAnnotationAction = (new ADVAnnotationCreation(this))->getCreateAnnotationAction();
    
    annotationSettingsAction  = new QAction(QIcon(":core/images/annotation_settings.png"), tr("Annotations highlighting..."), this);
    connect(annotationSettingsAction, SIGNAL(triggered()), SLOT(sl_onAnnotationSettings()));

    posSelectorAction = new QAction(QIcon(":core/images/goto.png"), tr("Go to position..."), this);
    posSelectorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    posSelectorAction->setShortcutContext(Qt::WindowShortcut);
    posSelectorAction->setObjectName(ADV_GOTO_ACTION);
    connect(posSelectorAction, SIGNAL(triggered()), SLOT(sl_onShowPosSelectorRequest()));

    toggleHLAction = new QAction("", this);
    connect(toggleHLAction, SIGNAL(triggered()), SLOT(sl_toggleHL()));

    syncViewManager = new ADVSyncViewManager(this);
    
    foreach(DNASequenceObject* dnaObj, dnaObjects) {
        addObject(dnaObj);
    }

    findDialogAction = new ADVGlobalAction(this, QIcon(":core/images/find_dialog.png"), tr("Find pattern..."), 10);
    findDialogAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    findDialogAction->setShortcutContext(Qt::WindowShortcut);
    connect(findDialogAction, SIGNAL(triggered()), SLOT(sl_onFindDialog()));

    addSequencePart = new QAction(tr("Insert subsequence..."), this);
    connect(addSequencePart, SIGNAL(triggered()), this, SLOT(sl_addSequencePart()));

    removeSequencePart = new QAction(tr("Remove subsequence..."), this);
    connect(removeSequencePart, SIGNAL(triggered()), this, SLOT(sl_removeSequencePart()));

    replaceSequencePart = new QAction(tr("Replace subsequence..."), this);
    connect(replaceSequencePart, SIGNAL(triggered()), this, SLOT(sl_replaceSequencePart()));

    removeSequenceObjectAction = new QAction(tr("Selected sequence from view"), this);
    connect(removeSequenceObjectAction, SIGNAL(triggered()), SLOT(sl_removeSelectedSequenceObject()));
    
    SecStructPredictViewAction::createAction(this);

}

QWidget* AnnotatedDNAView::createWidget() {
    GTIMER(c1,t1,"AnnotatedDNAView::createWidget");
    assert(scrollArea == NULL);
    
    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->setObjectName("annotated_DNA_splitter");
    //mainSplitter->setOpaqueResize(false);
    mainSplitter->setMaximumHeight(200);
    connect(mainSplitter, SIGNAL(splitterMoved(int, int)), SLOT(sl_splitterMoved(int, int)));

    mainSplitter->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mainSplitter, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(sl_onContextMenuRequested(const QPoint &)));

    scrollArea = new QScrollArea();
    scrollArea->setObjectName("annotated_DNA_scrollarea");
    scrollArea->setWidgetResizable(true);
    mainSplitter->addWidget(scrollArea);
    mainSplitter->setCollapsible(mainSplitter->indexOf(scrollArea), false);
    mainSplitter->setStretchFactor(mainSplitter->count()-1, 5);
    
    scrolledWidget = new QWidget(scrollArea);
    scrolledWidgetLayout = new QVBoxLayout();
    scrolledWidgetLayout->setContentsMargins(0, 0, 0, 0);
    scrolledWidgetLayout->setSpacing(0);
    scrolledWidget->setBackgroundRole(QPalette::Light);

    for (int i = seqContexts.size(); --i>=0;) { //use reverse mode -> so sequence widget for a lower index will be on top
        ADVSequenceObjectContext* seqCtx = seqContexts[i];
        ADVSingleSequenceWidget* block = new ADVSingleSequenceWidget(seqCtx, this);
        block->setObjectName("ADV_single_sequence_widget_"+QString::number(i));
        addSequenceWidget(block);
    }

    annotationsView = new AnnotationsTreeView(this);
    annotationsView->setObjectName("annotations_tree_view");
    mainSplitter->addWidget(annotationsView);
    mainSplitter->setCollapsible(mainSplitter->indexOf(annotationsView), false);
    mainSplitter->setStretchFactor(mainSplitter->count()-1, 1);

    scrolledWidget->setLayout(scrolledWidgetLayout);
    scrolledWidget->setObjectName("scrolled_widget_layout");
    
    //TODO: scroll area does not restore focus for last active child widget after Alt-Tab...
    scrollArea->setWidget(scrolledWidget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    clipb = new ADVClipboard(this);
    
    mainSplitter->installEventFilter(this);
    mainSplitter->setAcceptDrops(true);
    
    if (!seqViews.isEmpty()) {
        setFocusedSequenceWidget(seqViews.last());
    }

//add view global shortcuts
    mainSplitter->addAction(toggleHLAction);
    mainSplitter->addAction(removeSequenceObjectAction);

    mainSplitter->setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::SEQUENCE).icon);

    return mainSplitter;
}

void AnnotatedDNAView::sl_splitterMoved(int, int) {
    // WORKAROUND: looks like a QT bug:
    // ADVSequenceWidgets get paint events as needed, but scrolledWidget is over-painted by splitter's handle
    // to reproduce it open any complex (like 3d structure) view and pull the splitter handle upward slowly
    // -> workaround: update geometry for scrollArea or repaint main splitter's ares (todo: recheck effect)
    mainSplitter->repaint(scrollArea->geometry());

    /*if (timerId!=0) {
        killTimer(timerId);
    }
    timerId = startTimer(100);*/
}

void AnnotatedDNAView::timerEvent(QTimerEvent*) {
    //see comment for sl_splitterMoved()
    assert(timerId!=0);
    killTimer(timerId);
    timerId = 0;

    QWidget* w = scrollArea;
    QRect orig = w->geometry();
    QRect tmp = orig; tmp.adjust(0, 0, 1, 1);
    w->setGeometry(tmp);
    w->setGeometry(orig);
}

void AnnotatedDNAView::updateScrollAreaHeight() {
    if (!scrolledWidget->isVisible()) {
        return;
    }

    int fw = scrollArea->frameWidth();
    int mh = fw * 2;
    foreach(ADVSequenceWidget* v, seqViews) {
        mh+=v->height();
    }
    scrollArea->setMaximumHeight(mh);
    scrolledWidgetLayout->activate();
}

AnnotatedDNAView::~AnnotatedDNAView() {
    delete posSelector;
}

bool AnnotatedDNAView::eventFilter(QObject* o, QEvent* e) {
    if (o == mainSplitter) {
        if (e->type() == QEvent::DragEnter || e->type() == QEvent::Drop) {
            QDropEvent* de = (QDropEvent*)e;
            const QMimeData* md = de->mimeData();
            const GObjectMimeData* gomd = qobject_cast<const GObjectMimeData*>(md);
            if (gomd != NULL) {
                if (e->type() == QEvent::DragEnter) {
                    de->acceptProposedAction();
                } else {
                    GObject* obj = gomd->objPtr.data();
                    if (obj!=NULL) {
                        QString err = tryAddObject(obj);
                        if (!err.isEmpty()) {
                            QMessageBox::critical(NULL, tr("Error!"), err);
                        } 
                    }
                }
            }
        } 
    } else if (e->type() == QEvent::Resize) {
        ADVSequenceWidget* v = qobject_cast<ADVSequenceWidget*>(o);
        if ( v!=NULL ) {
            updateScrollAreaHeight();
        }
    }

    return false;
}

void AnnotatedDNAView::setFocusedSequenceWidget(ADVSequenceWidget* v) {
    if (v == focusedWidget) {
        return;
    }
    ADVSequenceWidget* prevFocus = focusedWidget;
    focusedWidget = v;
    updateMultiViewActions();
    emit si_focusChanged(prevFocus, focusedWidget);
}

bool AnnotatedDNAView::onObjectRemoved(GObject* o) {
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        AnnotationTableObject* ao = qobject_cast<AnnotationTableObject*>(o);
        annotationSelection->removeObjectAnnotations(ao);
        foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
            if (seqCtx->getAnnotationObjects().contains(ao)) {
                seqCtx->removeAnnotationObject(ao);
                break;
            }
        }
        annotations.removeOne(ao);
        emit si_annotationObjectRemoved(ao);
    } else if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        DNASequenceObject* seqObj = qobject_cast<DNASequenceObject*>(o);
        ADVSequenceObjectContext* seqCtx = getSequenceContext(seqObj);
        if (seqCtx!=NULL) {
            foreach(ADVSequenceWidget* w, seqCtx->getSequenceWidgets()) {
                removeSequenceWidget(w);
            }
            QSet<AnnotationTableObject*> aObjs = seqCtx->getAnnotationObjects();
            foreach(AnnotationTableObject* ao, aObjs) {
                removeObject(ao);
            }
            seqContexts.removeOne(seqCtx);
            AutoAnnotationObject* aa = autoAnnotationsMap.take(seqCtx);
            emit si_annotationObjectRemoved(aa->getAnnotationObject());
            delete aa;
            delete seqCtx;
        }
    }

    GObjectView::onObjectRemoved(o);
    return seqContexts.isEmpty();
}

void AnnotatedDNAView::addADVAction(ADVGlobalAction* a1) {
    for (int i=0; i<advActions.size(); i++) {
        ADVGlobalAction* a2 = advActions[i];
        int p1 = a1->getPosition();
        int p2 = a2->getPosition();
        if (p1 < p2 || (p1 == p2 && a1->text() < a2->text())) {
            advActions.insert(i, a1);
            return;
        }
    }
    advActions.append(a1);
}

void AnnotatedDNAView::buildStaticToolbar(QToolBar* tb) {
    tb->addAction(createAnnotationAction);
    tb->addAction(annotationSettingsAction);

    tb->addSeparator();
    tb->addAction(clipb->getCopySequenceAction());
    tb->addAction(clipb->getCopyComplementAction());
    tb->addAction(clipb->getCopyTranslationAction());
    tb->addAction(clipb->getCopyComplementTranslationAction());
    tb->addAction(clipb->getCopyAnnotationSequenceAction());
    tb->addAction(clipb->getCopyAnnotationSequenceTranslationAction());
    tb->addSeparator();
    
    if (posSelector == NULL && !seqContexts.isEmpty()) {
        int len = seqContexts.first()->getSequenceLen();
        posSelector = new PositionSelector(tb, 1, len);
        connect(posSelector, SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));
        posSelectorWidgetAction = tb->addWidget(posSelector);
    }  else {
        tb->addAction(posSelectorWidgetAction);
    }
    
    tb->addSeparator();
    syncViewManager->updateToolbar1(tb);
    tb->addSeparator();

    foreach(ADVGlobalAction* a, advActions) {
        if (a->getFlags().testFlag(ADVGlobalActionFlag_AddToToolbar)) {
            tb->addAction(a);
        }
    }

    GObjectView::buildStaticToolbar(tb);

    tb->addSeparator();
    syncViewManager->updateToolbar2(tb);
}

void AnnotatedDNAView::buildStaticMenu(QMenu* m) {
    m->addAction(posSelectorAction);
    clipb->addCopyMenu(m);
    m->addSeparator();
    addAddMenu(m);
    addAnalyseMenu(m);
    addAlignMenu(m);
    addExportMenu(m);
    addRemoveMenu(m);
    addEditMenu(m);
    m->addSeparator();

 
    m->addAction(annotationSettingsAction);

    annotationsView->adjustStaticMenu(m);

    GObjectView::buildStaticMenu(m);
}

ADVSequenceWidget* AnnotatedDNAView::findSequenceWidgetByPos(const QPoint& globalPos) const {
    foreach(ADVSequenceWidget* slv, seqViews) {
        const QRect& rect = slv->rect();
        QPoint localPos = slv->mapFromGlobal(globalPos);
        if (rect.contains(localPos)) {
            return slv;
        }
    }
    return NULL;
}

void AnnotatedDNAView::addAnalyseMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Analyze"));
    am->menuAction()->setObjectName(ADV_MENU_ANALYSE);
    foreach(ADVGlobalAction* a, advActions) {
        if (a->getFlags().testFlag(ADVGlobalActionFlag_AddToAnalyseMenu)) {
            am->addAction(a);
        }
    }
}

void AnnotatedDNAView::addAddMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Add"));
    am->menuAction()->setObjectName(ADV_MENU_ADD);
    am->addAction(createAnnotationAction);
}

void AnnotatedDNAView::addExportMenu(QMenu* m) {
    QMenu* em = m->addMenu(tr("Export"));
    em->menuAction()->setObjectName(ADV_MENU_EXPORT); 
}


void AnnotatedDNAView::addAlignMenu(QMenu* m) {
    QMenu* am = m->addMenu(tr("Align"));
    am->menuAction()->setObjectName(ADV_MENU_ALIGN); 
}


void AnnotatedDNAView::addRemoveMenu(QMenu* m) {
    QMenu* rm = m->addMenu(tr("Remove"));
    rm->menuAction()->setObjectName(ADV_MENU_REMOVE);

    rm->addAction(removeSequenceObjectAction);
}

void AnnotatedDNAView::addEditMenu(QMenu* m) {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    DNASequenceObject *seqObj = seqCtx->getSequenceObject();
    Document *curDoc = seqObj->getDocument();
    QMenu* rm = m->addMenu(tr("Edit sequence"));
    
    if(curDoc->findGObjectByType(GObjectTypes::SEQUENCE).isEmpty() || curDoc->isStateLocked()){
        rm->setDisabled(true);
    }else{
        rm->setEnabled(true);
    };
    rm->menuAction()->setObjectName(ADV_MENU_EDIT);
    if (seqCtx->getSequenceSelection()->isEmpty()) {
        rm->addAction(addSequencePart);
    } else {
        rm->addAction(replaceSequencePart);
    }
    rm->addAction(removeSequencePart);
}

Task* AnnotatedDNAView::updateViewTask(const QString& stateName, const QVariantMap& stateData) {
    return new UpdateAnnotatedDNAViewTask(this, stateName, stateData);
}

QVariantMap AnnotatedDNAView::saveState() {
    if (closing) {
        return QVariantMap();
    }
    QVariantMap state = AnnotatedDNAViewState::saveState(this);
    foreach(ADVSequenceWidget* sw, seqViews) {
        sw->saveState(state);
    }
    foreach(ADVSplitWidget* w, splitWidgets) {
        w->saveState(state);
    }
    annotationsView->saveState(state);
    return state;
}

void AnnotatedDNAView::saveWidgetState() {
    annotationsView->saveWidgetState();
}


bool AnnotatedDNAView::canAddObject(GObject* obj) {
    if (GObjectView::canAddObject(obj)) {
        return true;
    }
    if (isChildWidgetObject(obj)) {
            return true;
    }
    if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
        return true;
    }
    if (obj->getGObjectType()!=GObjectTypes::ANNOTATION_TABLE) {
            return false;
    }
    //todo: add annotations related to sequence object not in view (sobj) and add 'sobj' too the view ?
    bool hasRelation = false;
    foreach(ADVSequenceObjectContext* soc, seqContexts) {
        if (obj->hasObjectRelation(soc->getSequenceObject(), GObjectRelationRole::SEQUENCE)) {
            hasRelation = true;
            break;
        }
    }
    return hasRelation;
}

bool AnnotatedDNAView::isChildWidgetObject(GObject* obj) const {
    foreach(ADVSequenceWidget* lv, seqViews) {
        if (lv->isWidgetOnlyObject(obj)) {
            return true;
        }
    }
    
    foreach(ADVSplitWidget* sw,  splitWidgets) {
        if (sw->acceptsGObject(obj)) {
            return true;
        }
    }
    
     return false;
}


void AnnotatedDNAView::addSequenceWidget(ADVSequenceWidget* v) {
    assert(!seqViews.contains(v));
    seqViews.append(v);
    QList<ADVSequenceObjectContext*> contexts = v->getSequenceContexts();
    foreach(ADVSequenceObjectContext* c, contexts) {
        c->addSequenceWidget(v);
        addAutoAnnotations(c);
    }
    scrolledWidgetLayout->insertWidget(0, v);
    v->setVisible(true);
    v->installEventFilter(this);
    updateScrollAreaHeight();
    updateMultiViewActions();
    emit si_sequenceWidgetAdded(v);
}


void AnnotatedDNAView::removeSequenceWidget(ADVSequenceWidget* v) {
    assert(seqViews.contains(v));
    int idx = seqViews.indexOf(v);
    assert(idx>=0);
    
    //fix focus
    if (focusedWidget == v) {
        if (idx + 1 < seqViews.size()) {
            setFocusedSequenceWidget(seqViews[idx+1]);
        } else if (idx -1 >= 0) {
            setFocusedSequenceWidget(seqViews[idx-1]);
        } else {
            setFocusedSequenceWidget(NULL);
        }
    }
    
    //remove widget
    seqViews.removeOne(v);
    v->hide();

    QList<ADVSequenceObjectContext*> contexts = v->getSequenceContexts();
    foreach(ADVSequenceObjectContext* c, contexts) {
        c->removeSequenceWidget(v);
    }
    updateMultiViewActions();
    emit si_sequenceWidgetRemoved(v);
    scrolledWidgetLayout->removeWidget(v);
    delete v;
    //v->deleteLater(); //problem: updates for 'v' after seqCtx is destroyed
    updateScrollAreaHeight();
}

void AnnotatedDNAView::updateMultiViewActions() {
    bool canRemoveFocusedSequence = seqViews.size() > 1 && focusedWidget != NULL && focusedWidget->getActiveSequenceContext() != NULL;
    removeSequenceObjectAction->setEnabled(canRemoveFocusedSequence);
}

void AnnotatedDNAView::sl_onContextMenuRequested(const QPoint & scrollAreaPos) {
    QMenu m;

    QPoint globalPos = scrollArea->mapToGlobal(scrollAreaPos);
    m.addAction(posSelectorAction); 
    m.addSeparator()->setObjectName("FIRST_SEP");
    clipb->addCopyMenu(&m);
    m.addSeparator()->setObjectName(ADV_MENU_SECTION1_SEP);
    addAddMenu(&m);
    addAnalyseMenu(&m);
    addAlignMenu(&m);
    addExportMenu(&m);
    addEditMenu(&m);
    addRemoveMenu(&m);
    m.addSeparator()->setObjectName(ADV_MENU_SECTION2_SEP);

    if (annotationSelection->getSelection().size() == 1) {
        Annotation* a = annotationSelection->getSelection().first().annotation;
        QString name = a->getAnnotationName();
        AnnotationSettings* as = AppContext::getAnnotationsSettingsRegistry()->getAnnotationSettings(name);
        if (as->visible) {
            toggleHLAction->setText(tr("Disable '%1' highlighting").arg(a->getAnnotationName()));
        } else {   
            toggleHLAction->setText(tr("Enable '%1' highlighting").arg(a->getAnnotationName()));
        }

        AnnotationSettings* asettings = AppContext::getAnnotationsSettingsRegistry()->getAnnotationSettings(a);
        QIcon icon = GUIUtils::createSquareIcon(asettings->color, 10);
        toggleHLAction->setIcon(icon);
        
        toggleHLAction->setObjectName("toggle_HL_action");
        m.addAction(toggleHLAction);
    }
    annotationSettingsAction->setObjectName("annotation_settings_action");
    m.addAction(annotationSettingsAction);

    if (focusedWidget!=NULL) {
        focusedWidget->buildPopupMenu(m);
    }
    emit si_buildPopupMenu(this, &m);

    m.exec(QCursor::pos());
}


void AnnotatedDNAView::sl_onFindDialog() {
    ADVSequenceObjectContext* c = getSequenceInFocus();
    if (c != NULL) {
        FindDialog::runDialog(c);
    }
}

void AnnotatedDNAView::sl_onAnnotationSettings() {
    AnnotationSettingsDialogController::run(scrollArea);
}

void AnnotatedDNAView::sl_toggleHL() {
    if (annotationSelection->isEmpty()) {
        return;
    }
    QString name = annotationSelection->getSelection().first().annotation->getAnnotationName();
    AnnotationSettings* as = AppContext::getAnnotationsSettingsRegistry()->getAnnotationSettings(name);
    as->visible = !as->visible;
    AppContext::getAnnotationsSettingsRegistry()->changeSettings(QList<AnnotationSettings*>()<<as, true);
}

QString AnnotatedDNAView::tryAddObject(GObject* o) {
    if (o->getGObjectType() == GObjectTypes::UNLOADED) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new AddToViewTask(this, o));
        return "";
    }
    QList<ADVSequenceObjectContext*> rCtx;
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        rCtx = findRelatedSequenceContexts(o);
        if (rCtx.isEmpty()) {
            //ask user if to create new association
            CreateObjectRelationDialogController d(o, getSequenceGObjectsWithContexts(), 
                GObjectRelationRole::SEQUENCE, true,
                tr("Select sequence to associate annotations with:"));
            d.exec();
            rCtx = findRelatedSequenceContexts(o);
            if (rCtx.isEmpty()) {
                return "";
            }
        }
    }
    return addObject(o);
}

QString AnnotatedDNAView::addObject(GObject* o) {
    QList<ADVSequenceObjectContext*> rCtx;
    if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        rCtx = findRelatedSequenceContexts(o);
        if (rCtx.isEmpty()) {
            return tr("No sequence object found for annotations");
        }
    } 
    QString res = GObjectView::addObject(o);
    if (!res.isEmpty()) {
        return res;
    }

    bool internalViewObject = isChildWidgetObject(o);
    if (internalViewObject) {
        return "";
    }
   
    if (o->getGObjectType() == GObjectTypes::SEQUENCE) {
        DNASequenceObject* dnaObj = qobject_cast<DNASequenceObject*>(o);
        ADVSequenceObjectContext* sc = new ADVSequenceObjectContext(this, dnaObj);
        seqContexts.append(sc);
        //if mainSplitter==NULL -> its view initialization and widgets will be added later
        if (mainSplitter!=NULL && !isChildWidgetObject(dnaObj)) { 
            ADVSingleSequenceWidget* block = new ADVSingleSequenceWidget(sc, this);
            block->setObjectName("ADV_single_sequence_widget_" + QString::number(seqViews.count()));
            addSequenceWidget(block);
        }
        addRelatedAnnotations(sc);
        emit si_sequenceAdded(sc);
    } else if (o->getGObjectType() == GObjectTypes::ANNOTATION_TABLE) {
        AnnotationTableObject* ao = qobject_cast<AnnotationTableObject*>(o);
        assert(ao!=NULL);
        annotations.append(ao);
        foreach(ADVSequenceObjectContext* sc, rCtx) {
            sc->addAnnotationObject(ao);
        }
        emit si_annotationObjectAdded(ao);

    }
    return "";
}

QList<ADVSequenceObjectContext*> AnnotatedDNAView::findRelatedSequenceContexts(GObject* obj) const {
    QList<GObject*> relatedObjects = GObjectUtils::selectRelations(obj, GObjectTypes::SEQUENCE, GObjectRelationRole::SEQUENCE, objects, UOF_LoadedOnly);
    QList<ADVSequenceObjectContext*> res;
    foreach(GObject* seqObj, relatedObjects) {
        DNASequenceObject* dnaObj = qobject_cast<DNASequenceObject*>(seqObj);
        ADVSequenceObjectContext* ctx = getSequenceContext(dnaObj);
        res.append(ctx);
    }
    return res;
}

void AnnotatedDNAView::sl_onPosChangeRequest(int pos) {
    uiLog.trace(QString("ADV: center change request: %1").arg(pos));
    ADVSequenceWidget* seqBlock = getSequenceWidgetInFocus();
    assert(seqBlock!=NULL);
    seqBlock->centerPosition(pos-1);
}

void AnnotatedDNAView::sl_onShowPosSelectorRequest() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    assert(seqCtx!=NULL);
    QDialog dlg(getWidget());
    dlg.setModal(true);
    dlg.setWindowTitle(tr("Go To"));
    std::auto_ptr<PositionSelector> ps(new PositionSelector(&dlg, 1, seqCtx->getSequenceLen(), true));
    connect(ps.get(), SIGNAL(si_positionChanged(int)), SLOT(sl_onPosChangeRequest(int)));
    dlg.exec();
}

void AnnotatedDNAView::insertWidgetIntoSplitter(ADVSplitWidget* splitWidget) {
    assert(mainSplitter!=NULL);
    mainSplitter->insertWidget(0, splitWidget);
    mainSplitter->setStretchFactor(0, 1);
    splitWidgets.append(splitWidget);
}

void AnnotatedDNAView::unregisterSplitWidget(ADVSplitWidget* splitWidget) {
    splitWidgets.removeOne(splitWidget);
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(AnnotationTableObject* obj) const {
    assert(getAnnotationObjects(true).contains(obj));
    foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
        QSet<AnnotationTableObject*> aObjs = seqCtx->getAnnotationObjects(true);
        if(aObjs.contains(obj)) {
            return seqCtx;
        }
    }
    return NULL;
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceInFocus() const {
    ADVSequenceWidget* w = getSequenceWidgetInFocus();
    return w == NULL ? NULL : w->getActiveSequenceContext();
}

QList<ADVSequenceObjectContext*> AnnotatedDNAView::getAllSeqContextsInFocus() const {
    return getSequenceWidgetInFocus()->getSequenceContexts();
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(DNASequenceObject* obj) const {
    foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
        if(seqCtx->getSequenceObject() == obj) {
            return seqCtx;
        }
    }
    return NULL;
}

ADVSequenceObjectContext* AnnotatedDNAView::getSequenceContext(const GObjectReference& r) const {
    foreach(ADVSequenceObjectContext* seqCtx, seqContexts) {
        GObjectReference ref(seqCtx->getSequenceObject());
        if (ref == r) {
            return seqCtx;
        }
    }
    return NULL;
}

void AnnotatedDNAView::addRelatedAnnotations(ADVSequenceObjectContext* seqCtx) {
    QList<GObject*> allLoadedAnnotations = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE);
    QList<GObject*> annotations = GObjectUtils::findObjectsRelatedToObjectByRole(seqCtx->getSequenceObject(), 
                                    GObjectTypes::ANNOTATION_TABLE, GObjectRelationRole::SEQUENCE, 
                                    allLoadedAnnotations, UOF_LoadedOnly);

    foreach(GObject* ao, annotations) {
        if (objects.contains(ao)) {
            seqCtx->addAnnotationObject(qobject_cast<AnnotationTableObject*>(ao));
        } else {
            addObject(ao);
        }
    }
}


void AnnotatedDNAView::addAutoAnnotations( ADVSequenceObjectContext* seqCtx )
{
    
    AutoAnnotationObject* aa = new AutoAnnotationObject(seqCtx->getSequenceObject());
    seqCtx->addAutoAnnotationObject(aa->getAnnotationObject());
    autoAnnotationsMap.insert(seqCtx, aa);

    emit si_annotationObjectAdded(aa->getAnnotationObject());

    foreach(ADVSequenceWidget* w, seqCtx->getSequenceWidgets()) {
        AutoAnnotationsADVAction* aaAction = new AutoAnnotationsADVAction(w, aa);
        w->addADVSequenceWidgetAction( aaAction );
    }

}


void AnnotatedDNAView::sl_onDocumentAdded(Document* d) {
    GObjectView::sl_onDocumentAdded(d);
    importDocAnnotations(d);
}
 
void AnnotatedDNAView::importDocAnnotations(Document* doc) {
    QList<GObject*> docObjects = doc->getObjects();
    foreach(GObject* o, docObjects) {
        if (o->getGObjectType() != GObjectTypes::ANNOTATION_TABLE) {
            continue;
        }
        QList<ADVSequenceObjectContext*> cList = findRelatedSequenceContexts(o);
        if (cList.isEmpty()) {
            continue;
        }
        addObject(o);
    }
}

void AnnotatedDNAView::sl_onDocumentLoadedStateChanged() {
    Document* d = qobject_cast<Document*>(sender());
    importDocAnnotations(d);
    GObjectView::sl_onDocumentLoadedStateChanged();
}

QList<DNASequenceObject*> AnnotatedDNAView::getSequenceObjectsWithContexts() const {
    QList<DNASequenceObject*> res;
    foreach(ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

QList<GObject*> AnnotatedDNAView::getSequenceGObjectsWithContexts() const {
    QList<GObject*> res;
    foreach(ADVSequenceObjectContext* cx, seqContexts) {
        res.append(cx->getSequenceObject());
    }
    return res;
}

void AnnotatedDNAView::updateState(const AnnotatedDNAViewState& s) {
    if (!s.isValid()) {
        return;
    }
    QList<GObjectReference> objs =  s.getSequenceObjects();
    QVector<U2Region> regs =  s.getSequenceSelections();
    assert(objs.size() == regs.size());
    
    //TODO: sync seq object lists
    //TODO: sync annotation object lists

    for (int i=0; i < objs.size(); i++) {
        const GObjectReference& ref = objs[i];
        const U2Region& reg = regs[i];
        ADVSequenceObjectContext* seqCtx = getSequenceContext(ref);
        if (seqCtx == NULL) {
            continue;
        }
        U2Region wholeSeq(0, seqCtx->getSequenceLen());
        U2Region finalSel = reg.intersect(wholeSeq);
        seqCtx->getSequenceSelection()->clear();
        if (!finalSel.isEmpty()) {
            seqCtx->getSequenceSelection()->addRegion(finalSel);
        }
    }
    foreach(ADVSequenceWidget* sw, seqViews) {
        sw->updateState(s.stateData);
    }

    foreach (ADVSplitWidget* w, splitWidgets) {
        w->updateState(s.stateData);
    }
    
    annotationsView->updateState(s.stateData);
}

void AnnotatedDNAView::sl_addSequencePart(){
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    DNASequenceObject *seqObj = seqCtx->getSequenceObject();
    
    EditSequencDialogConfig cfg;

    cfg.mode = EditSequenceMode_Insert;
    cfg.source = U2Region(0, seqObj->getSequenceLen());
    cfg.alphabet = seqObj->getAlphabet();
        
    EditSequenceDialogController dialog(cfg, getSequenceWidgetInFocus());
    int result = dialog.exec();
    
    if(result == QDialog::Accepted){
        Task *t = new AddPartToSequenceTask(dialog.getDocumentFormatId(), seqObj, dialog.getPosToInsert(),
            dialog.getNewSequence(), dialog.getAnnotationStrategy(), dialog.getDocumentPath(), dialog.mergeAnnotations());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
        AppContext::getTaskScheduler()->registerTopLevelTask(t);
    }
}

void AnnotatedDNAView::sl_removeSequencePart(){
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    DNASequenceObject *seqObj = seqCtx->getSequenceObject();

    Document *curDoc = seqObj->getDocument();
    QString filter = DialogUtils::prepareDocumentsFileFilter(curDoc->getDocumentFormat()->getFormatId(), false);
    U2Region source(0, seqObj->getSequenceLen());

    U2Region selection = source;
    if( seqCtx->getSequenceSelection()->getSelectedRegions().size() > 0){
        selection = seqCtx->getSequenceSelection()->getSelectedRegions().first();
    }

    RemovePartFromSequenceDialogController dialog(selection, source, curDoc->getURLString(), getSequenceWidgetInFocus());
    int result = dialog.exec();
    if (result != QDialog::Accepted) {
        return;
    }
    Task *t;
    if(dialog.modifyCurrentDocument()){
        t = new RemovePartFromSequenceTask(dialog.getDocumentFormatId(), seqObj, dialog.getRegionToDelete(), dialog.getStrategy(), seqObj->getDocument()->getURL());
        connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    }else{
        t = new RemovePartFromSequenceTask(dialog.getDocumentFormatId(), seqObj, dialog.getRegionToDelete(), dialog.getStrategy(), dialog.getNewDocumentPath(), dialog.mergeAnnotations());
    }
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_replaceSequencePart() {
    ADVSequenceObjectContext* seqCtx = getSequenceInFocus();
    DNASequenceObject *seqObj = seqCtx->getSequenceObject();

    EditSequencDialogConfig cfg;

    cfg.mode = EditSequenceMode_Replace;
    cfg.source = U2Region(0, seqObj->getSequenceLen());
    cfg.alphabet = seqObj->getAlphabet();
    U2Region selection = seqCtx->getSequenceSelection()->getSelectedRegions().first();
    cfg.initialText = seqObj->getSequence().mid(selection.startPos, selection.length);

    EditSequenceDialogController dlg(cfg, getSequenceWidgetInFocus());
    int result = dlg.exec();
    
    if (result != QDialog::Accepted){
        return;
    }
    Task *t = new ReplacePartOfSequenceTask(dlg.getDocumentFormatId(), seqObj, selection, dlg.getNewSequence(), 
        dlg.getAnnotationStrategy(), dlg.getDocumentPath(), dlg.mergeAnnotations());
    connect(t, SIGNAL(si_stateChanged()), SLOT(sl_sequenceModifyTaskStateChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    seqCtx->getSequenceSelection()->clear();
}

void AnnotatedDNAView::sl_removeSelectedSequenceObject() {
    ADVSequenceWidget* sw = getSequenceWidgetInFocus();
    ADVSequenceObjectContext* soc = sw->getActiveSequenceContext();
    DNASequenceObject* so = soc->getSequenceObject();
    removeObject(so);
}

QList<AnnotationTableObject*> AnnotatedDNAView::getAnnotationObjects( bool includeAutoAnnotations /* = false*/ ) const
{
    QList<AnnotationTableObject*> result = annotations;
    if (includeAutoAnnotations) {
        foreach (AutoAnnotationObject* aa, autoAnnotationsMap.values()) {
            result += aa->getAnnotationObject();
        }
    }

    return result;
}

void AnnotatedDNAView::updateAutoAnnotations()
{
    QList<AutoAnnotationObject*> autoAnnotations = autoAnnotationsMap.values();
    foreach (AutoAnnotationObject* aa, autoAnnotations) {
        aa->update();
    }
}

void AnnotatedDNAView::sl_sequenceModifyTaskStateChanged()
{
    Task* t = qobject_cast<Task*> (sender());
    if (t == NULL) {
        return;
    }

    if (t->getState() == Task::State_Finished && !(t->hasError() || t->isCanceled()) ) {
        updateAutoAnnotations();
    }

}



}//namespace
