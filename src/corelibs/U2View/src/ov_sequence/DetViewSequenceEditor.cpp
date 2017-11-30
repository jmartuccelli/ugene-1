/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "DetViewSequenceEditor.h"

#include "DetView.h" // all should be astract! Do not use direct reference

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Msa.h>

#include <U2View/SequenceObjectContext.h>

#include <U2View/ADVSequenceWidget.h>
#include <U2View/AnnotatedDNAView.h>


namespace U2 {
DetViewSequenceEditor::DetViewSequenceEditor(DetView* _view)
    : view(_view),
      cursorColor(Qt::black),
      animationTimer(this) {
    reset();
    connect(&animationTimer, SIGNAL(timeout()), SLOT(sl_changeCursorColor()));
}

DetViewSequenceEditor::~DetViewSequenceEditor() {
    animationTimer.stop();
}

void DetViewSequenceEditor::reset() {
    cursor = view->getVisibleRange().startPos;
}

bool DetViewSequenceEditor::eventFilter(QObject *, QEvent *event) {
    CHECK(event->type() != QEvent::Wheel, false);
    CHECK(event->type() != QEvent::ContextMenu, false);

    SequenceObjectContext* ctx = view->getSequenceContext();
    const QList<ADVSequenceWidget*> list = ctx->getSequenceWidgets();
    SAFE_POINT(!list.isEmpty(), "seq wgts list is empty", false);
    ADVSequenceWidget* wgt = list.first();
    AnnotatedDNAView* dnaView = wgt->getAnnotatedDNAView();
    QAction* a = dnaView->removeAnnsAndQsAction;
    if (event->type() == QEvent::FocusOut) {
        // return delete
        a->setShortcut(QKeySequence(Qt::Key_Delete));
    }
    if (event->type() == QEvent::FocusIn) {
        // block delete again
        a->setShortcut(QKeySequence());
    }

    // TODO_SVEDIT: shift button!
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        SAFE_POINT(mouseEvent != NULL, "", true);

        qint64 pos = view->getRenderArea()->coordToPos(view->toRenderAreaPoint(mouseEvent->pos()));
        setCursor(pos); // use navigate and take shift into account
        return false;
    }

    // TODO_SVEDIT: separate methods
    if (event->type() == QEvent::KeyPress) {
        // set cursor position
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        SAFE_POINT(keyEvent != NULL, "", true);

        int key = keyEvent->key();
        bool shiftPressed = keyEvent->modifiers().testFlag(Qt::ShiftModifier);
        switch (key) {
        case Qt::Key_Left:
            navigate(cursor - 1, shiftPressed);
            break;
        case Qt::Key_Right:
            navigate(cursor + 1, shiftPressed);
            break;
        case Qt::Key_Up:
            if (view->isWrapMode()) {
                navigate(cursor - view->getSymbolsPerLine(), shiftPressed);
            }
            break;
        case Qt::Key_Down:
            if (view->isWrapMode()) {
                navigate(cursor + view->getSymbolsPerLine(), shiftPressed);
            }
            break;
        case Qt::Key_Home:
            navigate(0, shiftPressed);
            break;
        case Qt::Key_End:
            navigate(view->getSequenceLength(), shiftPressed);
            break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            deleteChar(key);
            break;
        case Qt::Key_Space:
            insertChar(U2Msa::GAP_CHAR);
            break;
        default:
            if (key >= Qt::Key_A && key <= Qt::Key_Z) {
                insertChar(key);
            }
        }
    }
    return true;
}

void DetViewSequenceEditor::setCursor(int newPos) {
    CHECK(newPos >= 0 && newPos <= view->getSequenceLength(), );
    if (cursor != newPos) {
        cursor = newPos;
        view->ensureVisible(cursor);
        view->update();
    }
}

void DetViewSequenceEditor::navigate(int newPos, bool shiftPressed) {
    CHECK(newPos != cursor, );
    CHECK(newPos >= 0 && newPos <= view->getSequenceLength(), );

    DNASequenceSelection* selection = view->getSequenceContext()->getSequenceSelection();
    if (shiftPressed) {
        int extension = qAbs(cursor - newPos);
        if (selection->isEmpty()) {
            // if selection is empty - start a new one!
            selection->setRegion(U2Region(qMin(cursor, newPos), extension));
        } else {
            // expand selection
            U2Region r = selection->getSelectedRegions().first();
            selection->clear();

            if (r.contains(newPos) || (r.endPos() == newPos) ) {
                if (r.length - extension != 0) {
                    // shorten the selection
                    if (r.startPos == cursor) {
                        selection->setRegion(U2Region(newPos, r.length - extension));
                    } else {
                        selection->setRegion(U2Region(r.startPos, r.length - extension ));
                    }
                }
            } else {
                if (newPos < r.startPos && cursor == r.endPos()) {
                    selection->setRegion(U2Region(newPos, r.startPos - newPos));
                } else if (newPos > r.endPos() && cursor == r.startPos) {
                    selection->setRegion(U2Region(r.endPos(), newPos - r.endPos()));
                } else {
                    if (r.startPos == cursor) {
                        selection->setRegion(U2Region(newPos, r.length + extension));
                    } else {
                        selection->setRegion(U2Region(r.startPos, r.length + extension ));
                    }
                }
            }
        }
        setCursor(newPos);
    } else {
        selection->clear();
        setCursor(newPos);
    }
}

void DetViewSequenceEditor::insertChar(int character) {
    U2SequenceObject* seqObj = view->getSequenceObject();
    SAFE_POINT(seqObj != NULL, "SeqObject is NULL", );
    U2OpStatusImpl os;
    const DNASequence seq(QByteArray(1, character));
    U2Region r;
    SequenceObjectContext* ctx = view->getSequenceContext();
    SAFE_POINT(ctx != NULL, "SequenceObjectContext", );
    if (ctx->getSequenceSelection()->isEmpty()) {
        r = U2Region(cursor, 0);
    } else {
        // ignore multuselection for now
        r = ctx->getSequenceSelection()->getSelectedRegions().first();
    }

    seqObj->replaceRegion(r, seq, os);
    navigate(cursor + 1, false);
}

// TODO_SVEDIT: rename
void DetViewSequenceEditor::deleteChar(int key) {
    CHECK(key == Qt::Key_Backspace || key == Qt::Key_Delete, ); // safe_point
    U2SequenceObject* seqObj = view->getSequenceObject();
    SAFE_POINT(seqObj != NULL, "SeqObject is NULL", );

    // add safe points
    U2OpStatusImpl os;
    U2Region regionToRemove;
    if (cursor > 0) {
        regionToRemove = U2Region(key == Qt::Key_Backspace ? cursor - 1 : cursor, 1);
    }
    SequenceObjectContext* ctx = view->getSequenceContext();
    if (!ctx->getSequenceSelection()->isEmpty()) {
        // remove the selection
        // check the count of region and remove from the end!
        foreach (U2Region r, ctx->getSequenceSelection()->getSelectedRegions()) {
            seqObj->removeRegion(os, r);
            regionToRemove = r; // TODO_SVEDIT: check
        }
        ctx->getSequenceSelection()->clear();
        // TODO_SVEDIT: set up cursor position
        return;
    }

    if (regionToRemove.length == view->getSequenceLength()) {
        // you're going to remove the whole sequence
        Document* doc = seqObj->getDocument();
        SAFE_POINT(doc != NULL, "Document is NULL", );
        doc->removeObject(seqObj);
        return;
    }
    seqObj->removeRegion(os, regionToRemove);

    SAFE_POINT_OP(os, );
    setCursor(key == Qt::Key_Backspace ? cursor - 1 : cursor);
}

void DetViewSequenceEditor::sl_editMode(bool active) {
    SequenceObjectContext* ctx = view->getSequenceContext();
    const QList<ADVSequenceWidget*> list = ctx->getSequenceWidgets();
    SAFE_POINT(!list.isEmpty(), "seq wgts list is empty", );
    ADVSequenceWidget* wgt = list.first();
    AnnotatedDNAView* dnaView = wgt->getAnnotatedDNAView();
    QAction* a = dnaView->removeAnnsAndQsAction;

    if (active) {
        // deactivate Delete shortcut
        a->setShortcut(QKeySequence());
        reset();
        view->installEventFilter(this);
        animationTimer.start(500);
    } else {
        view->removeEventFilter(this);
        a->setShortcut(QKeySequence(Qt::Key_Delete));
        animationTimer.stop();
        view->update();
    }
}

void DetViewSequenceEditor::sl_changeCursorColor() {
    cursorColor = (cursorColor == QColor(Qt::black)) ? Qt::darkGray : Qt::black;
    view->update();
}

} // namespace