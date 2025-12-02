#pragma once

// Embedded icon data for the Bus Controller application
// These icons are embedded directly into the binary to avoid shipping separate PNG files

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/mstream.h>

/* XPM - Up Arrow Light (for dark theme) */
static const char *up_arrow_xpm[] = {"24 24 3 1", // NOLINT
                                     "  c None",
                                     ". c #FFFFFF",
                                     "# c #CCCCCC",
                                     "                        ",
                                     "                        ",
                                     "           ..           ",
                                     "          ....          ",
                                     "         ......         ",
                                     "        ........        ",
                                     "       ..........       ",
                                     "      ............      ",
                                     "     ..............     ",
                                     "    ................    ",
                                     "   ..................   ",
                                     "  ....................  ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "        ........        ",
                                     "                        ",
                                     "                        "};

/* XPM - Up Arrow Dark (for light theme) */
static const char *up_arrow_dark_xpm[] = {"24 24 3 1", // NOLINT
                                          "  c None",
                                          ". c #333333",
                                          "# c #555555",
                                          "                        ",
                                          "                        ",
                                          "           ..           ",
                                          "          ....          ",
                                          "         ......         ",
                                          "        ........        ",
                                          "       ..........       ",
                                          "      ............      ",
                                          "     ..............     ",
                                          "    ................    ",
                                          "   ..................   ",
                                          "  ....................  ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "        ........        ",
                                          "                        ",
                                          "                        "};

/* XPM - Down Arrow Light (for dark theme) */
static const char *down_arrow_xpm[] = {"24 24 3 1", // NOLINT
                                       "  c None",
                                       ". c #FFFFFF",
                                       "# c #CCCCCC",
                                       "                        ",
                                       "                        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "        ........        ",
                                       "  ....................  ",
                                       "   ..................   ",
                                       "    ................    ",
                                       "     ..............     ",
                                       "      ............      ",
                                       "       ..........       ",
                                       "        ........        ",
                                       "         ......         ",
                                       "          ....          ",
                                       "           ..           ",
                                       "                        ",
                                       "                        "};

/* XPM - Down Arrow Dark (for light theme) */
static const char *down_arrow_dark_xpm[] = {"24 24 3 1", // NOLINT
                                            "  c None",
                                            ". c #333333",
                                            "# c #555555",
                                            "                        ",
                                            "                        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "        ........        ",
                                            "  ....................  ",
                                            "   ..................   ",
                                            "    ................    ",
                                            "     ..............     ",
                                            "      ............      ",
                                            "       ..........       ",
                                            "        ........        ",
                                            "         ......         ",
                                            "          ....          ",
                                            "           ..           ",
                                            "                        ",
                                            "                        "};

/* XPM - Remove/X Light (for dark theme) */
static const char *remove_xpm[] = {"24 24 3 1", // NOLINT
                                   "  c None",
                                   ". c #FF6666",
                                   "# c #FF4444",
                                   "                        ",
                                   "                        ",
                                   "   ..              ..   ",
                                   "   ....          ....   ",
                                   "    ....        ....    ",
                                   "     ....      ....     ",
                                   "      ....    ....      ",
                                   "       ....  ....       ",
                                   "        ........        ",
                                   "         ......         ",
                                   "          ....          ",
                                   "          ....          ",
                                   "         ......         ",
                                   "        ........        ",
                                   "       ....  ....       ",
                                   "      ....    ....      ",
                                   "     ....      ....     ",
                                   "    ....        ....    ",
                                   "   ....          ....   ",
                                   "   ..              ..   ",
                                   "                        ",
                                   "                        ",
                                   "                        ",
                                   "                        "};

/* XPM - Remove/X Dark (for light theme) */
static const char *remove_dark_xpm[] = {"24 24 3 1", // NOLINT
                                        "  c None",
                                        ". c #CC3333",
                                        "# c #AA2222",
                                        "                        ",
                                        "                        ",
                                        "   ..              ..   ",
                                        "   ....          ....   ",
                                        "    ....        ....    ",
                                        "     ....      ....     ",
                                        "      ....    ....      ",
                                        "       ....  ....       ",
                                        "        ........        ",
                                        "         ......         ",
                                        "          ....          ",
                                        "          ....          ",
                                        "         ......         ",
                                        "        ........        ",
                                        "       ....  ....       ",
                                        "      ....    ....      ",
                                        "     ....      ....     ",
                                        "    ....        ....    ",
                                        "   ....          ....   ",
                                        "   ..              ..   ",
                                        "                        ",
                                        "                        ",
                                        "                        ",
                                        "                        "};

// Helper function to get the appropriate icon based on theme
inline wxBitmap getUpArrowBitmap(bool isDarkTheme) { return wxBitmap(isDarkTheme ? up_arrow_xpm : up_arrow_dark_xpm); }

inline wxBitmap getDownArrowBitmap(bool isDarkTheme) { return wxBitmap(isDarkTheme ? down_arrow_xpm : down_arrow_dark_xpm); }

inline wxBitmap getRemoveBitmap(bool isDarkTheme) { return wxBitmap(isDarkTheme ? remove_xpm : remove_dark_xpm); }
