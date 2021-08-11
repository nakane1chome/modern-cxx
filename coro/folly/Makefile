FOLLY_DIR=${HOME}/folly/
RANGES_DIR=${HOME}/range-v3/

CXXFLAGS=\
	-std=c++20 \
	-fcoroutines \
	-I${FOLLY_DIR}/include/ \
	-I${RANGES_DIR}/include/

CPPFLAGS=-DGFLAGS_IS_A_DLL=0 -DFMT_LOCALE

# Need to use the same version that was compiled with folly
FMT_LIB=libfmt.a

LDFLAGS=-L${FOLLY_DIR}/lib/ 
LIBS=\
	-lfolly \
	-lglog \
	-lgflags  \
	-pthread \
	-lunwind \
	-ldwarf \
	-ldl \
	-levent  \
	-lm  \
	-ldouble-conversion \
	-liberty \
	-lboost_context \
	${FMT_LIB}
CXX=g++-11
LD=${CXX}

main : main.o
	${LD} ${LDFLAGS} $< -o $@ ${LIBS}



