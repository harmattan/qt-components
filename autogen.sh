TOPDIR=$PWD
echo -n "Generating fremantle sources for ssu and cssu targets..."
cd src && ./autogen.sh >/dev/null 2>&1; cd $TOPDIR
echo "done"

for example in HelloWorld TutorialApplication QmlComponentGallery QmlComponentExtrasGallery; do
    echo -n "Generating fremantle" $example "example..."
    cd examples/fremantle/$example && ./autogen.sh >/dev/null 2>&1; cd $TOPDIR
    echo "done"
done

echo ""
echo "type ./configure --help for a set of options"