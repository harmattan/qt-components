TEMPLATE = subdirs
SUBDIRS = components

meego {
    SUBDIRS += meego
    extras:SUBDIRS += meego/extras
}

fremantle {
    SUBDIRS += fremantle
    extras:SUBDIRS += fremantle/extras
}

symbian3 {
    SUBDIRS += symbian
    extras:SUBDIRS += symbian/extras
}
