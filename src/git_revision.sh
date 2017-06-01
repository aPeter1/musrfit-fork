#!/bin/bash

echo "Generating header for git hash" $1 $2
GIT_HEADER="$1/$2"
if [ -z "$2" ]; then
        GIT_HEADER="$1/git-revision.h"
fi

GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`

GIT_VERSION=`git log -n 1 --pretty=format:"%ad - %h"`
if grep --quiet "$GIT_VERSION" $GIT_HEADER; then
        echo "No need to generate new $GIT_HEADER - git hash is unchanged"
        exit 0;
fi

echo "git branch is  : " $GIT_BRANCH
echo "git version is : " $GIT_VERSION

echo "#ifndef GIT_VERSION_H" > $GIT_HEADER
echo "#define GIT_VERSION_H" >> $GIT_HEADER
echo "" >> $GIT_HEADER
echo "#define GIT_BRANCH \"$GIT_BRANCH\"" >> $GIT_HEADER
echo "#define GIT_CURRENT_SHA1 \"$GIT_VERSION\"" >> $GIT_HEADER
echo "" >> $GIT_HEADER
echo "#endif //GIT_VERSION_H" >> $GIT_HEADER

echo "file is generated into" $GIT_HEADER
