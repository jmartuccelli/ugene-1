include (U2Formats.pri)

# Input
HEADERS += src/ABIFormat.h \
           src/AbstractDifferentialFormat.h \
           src/AbstractVariationFormat.h \
           src/ACEFormat.h \
           src/ASNFormat.h \
           src/BAMUtils.h \
           src/BedFormat.h \
           src/ClustalWAlnFormat.h \
           src/ColumnDataParser.h \
           src/DifferentialExpressionFormat.h \
           src/DNAQualityIOUtils.h \
           src/DocumentFormatUtils.h \
           src/EMBLGenbankAbstractDocument.h \
           src/EMBLPlainTextFormat.h \
           src/FastaFormat.h \
           src/FastqFormat.h \
           src/FpkmTrackingFormat.h \
           src/GenbankFeatures.h \
           src/GenbankLocationParser.h \
           src/GenbankPlainTextFormat.h \
           src/GFFFormat.h \
           src/GTFFormat.h \
           src/IOLibUtils.h \
           src/MegaFormat.h \
           src/MSFFormat.h \
           src/NewickFormat.h \
           src/NEXUSFormat.h \
           src/NEXUSParser.h \
           src/PDBFormat.h \
           src/PDWFormat.h \
           src/PlainTextFormat.h \
           src/RawDNASequenceFormat.h \
           src/SAMFormat.h \
           src/SCFFormat.h \
           src/SimpleSNPVariationFormat.h \
           src/StdResidueDictionary.h \
           src/StockholmFormat.h \
           src/StreamSequenceReader.h \
           src/StreamSequenceWriter.h \
           src/SwissProtPlainTextFormat.h \
           src/VCF4VariationFormat.h \
           src/WIGFormat.h \
           src/sqlite_dbi/SQLiteAnnotationDbi.h \
           src/sqlite_dbi/SQLiteAssemblyDbi.h \
           src/sqlite_dbi/SQLiteAttributeDbi.h \
           src/sqlite_dbi/SQLiteDbi.h \
           src/sqlite_dbi/SQLiteFeatureDbi.h \
           src/sqlite_dbi/SQLiteMsaDbi.h \
           src/sqlite_dbi/SQLiteObjectDbi.h \
           src/sqlite_dbi/SQLiteSequenceDbi.h \
           src/sqlite_dbi/SQLiteVariantDbi.h \
           src/tasks/ConvertAssemblyToSamTask.h \
           src/sqlite_dbi/assembly/AssemblyPackAlgorithm.h \
           src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.h \
           src/sqlite_dbi/assembly/RTreeAssemblyAdapter.h \
           src/sqlite_dbi/assembly/SingleTableAssemblyAdapter.h
SOURCES += src/ABIFormat.cpp \
           src/AbstractDifferentialFormat.cpp \
           src/AbstractVariationFormat.cpp \
           src/ACEFormat.cpp \
           src/ASNFormat.cpp \
           src/BAMUtils.cpp \
           src/BedFormat.cpp \
           src/ClustalWAlnFormat.cpp \
           src/ColumnDataParser.cpp \
           src/DifferentialExpressionFormat.cpp \
           src/DNAQualityIOUtils.cpp \
           src/DocumentFormatUtils.cpp \
           src/EMBLGenbankAbstractDocument.cpp \
           src/EMBLPlainTextFormat.cpp \
           src/FastaFormat.cpp \
           src/FastqFormat.cpp \
           src/FpkmTrackingFormat.cpp \
           src/GenbankFeatures.cpp \
           src/GenbankLocationParser.cpp \
           src/GenbankPlainTextFormat.cpp \
           src/GFFFormat.cpp \
           src/GTFFormat.cpp \
           src/MegaFormat.cpp \
           src/MSFFormat.cpp \
           src/NewickFormat.cpp \
           src/NEXUSFormat.cpp \
           src/PDBFormat.cpp \
           src/PDWFormat.cpp \
           src/PlainTextFormat.cpp \
           src/RawDNASequenceFormat.cpp \
           src/SAMFormat.cpp \
           src/SCFFormat.cpp \
           src/SimpleSNPVariationFormat.cpp \
           src/StdResidueDictionary.cpp \
           src/StockholmFormat.cpp \
           src/StreamSequenceReader.cpp \
           src/StreamSequenceWriter.cpp \
           src/SwissProtPlainTextFormat.cpp \
           src/VCF4VariationFormat.cpp \
           src/WIGFormat.cpp \
           src/sqlite_dbi/SQLiteAnnotationDbi.cpp \
           src/sqlite_dbi/SQLiteAssemblyDbi.cpp \
           src/sqlite_dbi/SQLiteAttributeDbi.cpp \
           src/sqlite_dbi/SQLiteDbi.cpp \
           src/sqlite_dbi/SQLiteFeatureDbi.cpp \
           src/sqlite_dbi/SQLiteMsaDbi.cpp \
           src/sqlite_dbi/SQLiteObjectDbi.cpp \
           src/sqlite_dbi/SQLiteSequenceDbi.cpp \
           src/sqlite_dbi/SQLiteVariantDbi.cpp \
           src/tasks/ConvertAssemblyToSamTask.cpp \
           src/sqlite_dbi/assembly/AssemblyPackAlgorithm.cpp \
           src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.cpp \
           src/sqlite_dbi/assembly/RTreeAssemblyAdapter.cpp \
           src/sqlite_dbi/assembly/SingleTableAssemblyAdapter.cpp
RESOURCES += U2Formats.qrc
TRANSLATIONS += transl/chinese.ts \
                transl/czech.ts \
                transl/english.ts \
                transl/russian.ts
