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

#include "SeqPosSupport.h"
#include "python/PythonSupport.h"
#include "R/RSupport.h"
#include "utils/ExternalToolUtils.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

namespace U2 {

SeqPosSupport::SeqPosSupport(const QString &name)
: ExternalTool(name)
{
    initialize();
}

void SeqPosSupport::initialize() {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "Cistrome";
    description = SeqPosSupport::tr("<i>SeqPos</i> - Finds motifs enriched in a set of regions.");

    executableFileName = "MDSeqPos.py";

    toolRunnerProgramm = ET_PYTHON;
    dependencies << ET_PYTHON
                 << ET_PYTHON_DJANGO
                 << ET_PYTHON_NUMPY
                 << ET_R
                 << ET_R_SEQLOGO;

    validMessage = "mdseqpos \\(official trunk\\):";
    validationArguments << "-v";

    versionRegExp=QRegExp("Version (\\d+\\.\\d+)");

    ExternalToolUtils::addDefaultCistromeDirToSettings();
    ExternalToolUtils::addCistromeDataPath(ASSEMBLY_DIR, ASSEMBLY_DIR_NAME, true);

    muted = true;
}

} // U2
