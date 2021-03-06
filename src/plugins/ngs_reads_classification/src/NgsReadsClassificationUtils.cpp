/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
 * http://ugene.net
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

#include <QFileInfo>

#include <U2Core/GUrlUtils.h>

#include "NgsReadsClassificationUtils.h"

namespace U2 {

QString NgsReadsClassificationUtils::getClassificationFileName(const QString &sourceFileUrl, const QString &toolName, const QString &extension, bool truncate) {
    QString baseName = GUrlUtils::getPairedFastqFilesBaseName(sourceFileUrl, truncate);
    return baseName + QString("_%1_classification.%2").arg(toolName).arg(extension);
}

int NgsReadsClassificationUtils::countClassified(const LocalWorkflow::TaxonomyClassificationResult& classification) {
    LocalWorkflow::TaxonomyClassificationResult::const_iterator it;
    int classifiedCount = 0;
    for (it = classification.constBegin(); it != classification.constEnd(); ++it) {
        if (it.value() != LocalWorkflow::TaxonomyTree::UNCLASSIFIED_ID && it.value() != LocalWorkflow::TaxonomyTree::UNDEFINED_ID) {
            classifiedCount++;
        }
    }

    return classifiedCount;
}

}   // namespace U2
