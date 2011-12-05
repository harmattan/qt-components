TOPDIR=$PWD
echo -n "Generating fremantle sources for ssu and cssu targets..."
cd src && ./autogen.sh >/dev/null 2>&1; cd $TOPDIR
echo "done"
echo -n "Generating fremantle examples..."
cd examples/fremantle/HelloWorld && ./autogen.sh >/dev/null 2>&1; cd $TOPDIR
echo "done"

echo ""
echo "type ./configure --help for a set of options"