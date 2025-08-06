#ifndef BASE_TYPE_H
#define BASE_TYPE_H

namespace BaseType {

enum FormatType {
    FormatUnknown, /**< Unknown / unsupported format. */
    FormatSTL,     /**< STL format. */
    FormatGLB,     /**< GLB format. */

};

/**
 * Action types used by action factories.
 */
enum ActionType {
    ActionNone, /**< Invalid action id. */

    ActionDefault,

    ActionFileNew,
    ActionFileOpen,
    ActionFileSave,
    ActionFileSaveAs,
    ActionFileExport,
    ActionFileClose,
    ActionFilePrint,
    ActionFilePrintPDF,
    ActionFilePrintPreview,

    ActionModelAddCube,
    ActionModelAddCylinder,

    ActionZoomIn,
    ActionZoomOut,
    ActionZoomAuto,
    ActionZoomWindow,
    ActionZoomPrevious,

    ActionSelect,
    ActionSelectSingle,
    ActionSelectContour,
    ActionSelectWindow,
    ActionDeselectWindow,
    ActionSelectAll,
    ActionDeselectAll,

    ActionLast
};

}; // namespace BaseType

#endif // BASE_TYPE_H