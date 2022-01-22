
if [ ! -z ${KRUNCHER_JOURNAL} ]; then
	mkdir -p ${KRUNCHER_JOURNAL}
fi

function ShowBuild
{
	echo -ne "\033[33mBuild Info\n\033[33m"
	for item in `find .. -name "flags.make"`; do
		echo -ne "\033[36m${item}\033[0m\n\033[3m"
		cat ${item}  | grep INCLUDES
	done
	echo -ne "\033[32m"
	for item in `find .. -name "link.txt"`; do
		echo -ne "\033[35m${item}\033[0m\n\033[3m"
		cat ${item} | grep "g++"
	done
	echo -ne "\033[0m"
}

function Install
{
	sudo cmake --install ../src.build
}

function Clean
{
	[ -d ../src.build ] && sudo rm -rf ../src.build
}



function Build
{
	[ -f prebuild ] && ./prebuild
	if [ "${1}" == "-clean" ]; then
		Clean
		shift
	fi

	mkdir -p ../src.build
	cmake -S .  -B  ../src.build
	cmake  --build ../src.build/ 
	[ "$?" != "0" ] && return 1


	if [ "${1}" == "-install" ]; then 
		Install
		shift
	fi
	[ -f postbuild ] && ./postbuild
	return 0
}


