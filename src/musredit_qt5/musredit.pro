TEMPLATE = subdirs

TARGET = musredit

SUBDIRS = \
	musrStep \
	musrWiz \
	musredit

# where to find the sub-projects - give the folders
musrStep.subdir = musrStep
musrWiz.subdir  = musrWiz
musredit.subdir = musredit

# what sub-project depends on others
musredit.depends = musrStep musrWiz

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered

