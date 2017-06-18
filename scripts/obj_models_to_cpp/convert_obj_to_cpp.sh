cp -T $1.obj $1.hpp

# replace new lines with @
sed -i -e ':a' -e 'N' -e '$!ba' -e 's/\n/@/g' $1.hpp

# delete comments
sed -i -e "s/#[^@]*@/@/g" $1.hpp

# delete s and g lines
sed -i -e "s/@[sg][^@]*/@/g" $1.hpp

# delete mtllib lines
sed -i -e "s/@mtllib[^@]*@/@/g" $1.hpp

# delete vt and vn lines
sed -i -e "s/v[tn][^@]*@//g" $1.hpp

# delete extra new lines
sed -i -e "s/@*@/@/g" $1.hpp
sed -i -e "s/^@//g" $1.hpp

# make start of coords array
sed -i -e "s/^/{fvec3 coords[]{@/g" $1.hpp

# make coords array
sed -i -e "s/v \([0-9.-]*\) \([0-9.-]*\) \([0-9.-]*\)@/{\1,\2,\3},@/g" $1.hpp

# make end of coords array and start of buffer init
sed -i -e "s/},@\([^{]\)/}};@LOAD_TO = new std::remove_pointer<decltype(LOAD_TO)>::type({@\1/g" $1.hpp

# make triangles array
sed -i -e "s~f \([0-9]*\)/[0-9]*/[0-9]* \([0-9]*\)/[0-9]*/[0-9]* \([0-9]*\)/[0-9]*/[0-9]*@~TRIANGLE(coords[\1-1],coords[\2-1],coords[\3-1]),@~g" $1.hpp

# make triangle materials
sed -i -e "s/usemtl \([A-z_]*\)/#ifdef TRIANGLE@#undef TRIANGLE@#endif@#define TRIANGLE MATERIAL_\1/g" $1.hpp

# make end of triangles array
sed -i -e "s/@$/@#ifdef TRIANGLE@#undef TRIANGLE@#endif@#undef LOAD_TO@});}/g" $1.hpp

# replace @ with new lines
sed -i -e "s/@/\n/g" $1.hpp
