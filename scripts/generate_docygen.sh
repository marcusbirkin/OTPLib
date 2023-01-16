#!/bin/bash
LIB_ROOT_PATH=$( cd "$(dirname "${BASH_SOURCE[0]}/..")" ; pwd -P )

echo -n "Looking for git..."
if [ -x "$(command -v git)" ]; then
    echo $(command -v git)
else
  echo "Error: git is not installed" >&2
  exit 1
fi
LIB_TAG=$( git describe --always --tags --abbrev=0 )
echo "Git reports repository tag $LIB_TAG"

echo -n "Looking for doxygen..."
if [ -x "$(command -v doxygen)" ]; then
    echo $(command -v doxygen)
else
  echo "Error: doxygen is not installed" >&2
  exit 1
fi

DOXYFILE="$LIB_ROOT_PATH/Doxyfile"
echo -n 'Looking for Doxyfile...'
if test -f "$DOXYFILE"; then
    echo "$DOXYFILE"
else
  echo "Error: Doxyfile not found in $LIB_ROOT_PATH" >&2
  exit 1
fi

echo "Generating public API documentation into $lib_root_path/docs/public_api"
( 
    cat "$DOXYFILE" ; 
    echo "PROJECT_NUMBER=$LIB_TAG" ;
) | doxygen -
if [ $? -ne 0 ]; then
  exit 1
fi


echo "Generating internal documentation into $lib_root_path/docs/internal"
( 
    cat "$DOXYFILE" ; 
    echo "PROJECT_NUMBER=$LIB_TAG (Internal documentation)" ;
    echo "HTML_OUTPUT=internal" ;  
    echo "EXTRACT_ALL=ON" ;
    echo "INTERNAL_DOCS=YES" ;
    echo "HIDE_UNDOC_MEMBERS=NO" ;
    echo "HIDE_UNDOC_CLASSES=NO" ;
    # These symbols are obvious and need no additonal documentation
    echo "EXCLUDE_SYMBOLS+=operator<<" ;
    echo "EXCLUDE_SYMBOLS+=operator>>" ;
    echo "EXCLUDE_SYMBOLS+=operator+=" ;
    echo "EXCLUDE_SYMBOLS+=operator==" ;
    echo "EXCLUDE_SYMBOLS+=operator!=" ;
    echo "EXCLUDE_SYMBOLS+=operator+" ;
    echo "EXCLUDE_SYMBOLS+=operator<" ;
    echo "EXCLUDE_SYMBOLS+=operator>" ;
    echo "EXCLUDE_SYMBOLS+=operator++" ;
    echo "EXCLUDE_SYMBOLS+=operator--" ;
    echo "EXCLUDE_SYMBOLS+=qHash" ;
) | doxygen -
if [ $? -ne 0 ]; then
  exit 1
fi