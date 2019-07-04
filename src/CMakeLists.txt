set( sankoff_hdr
  utils.h
  basetree.h
  sankoff.h
  )

set( sankoff_src
  sankoffmain.cpp
  utils.cpp
  basetree.cpp
  sankoff.cpp
  )

set( sample_sankoff_hdr
  utils.h
  basetree.h
  sankoff.h
  )

set( sample_sankoff_src
  samplesankoffmain.cpp
  utils.cpp
  basetree.cpp
  sankoff.cpp
  )

set( dimacs_hdr
  utils.h
  basetree.h
  dimacs.h
  sankoff.h
  )

set( dimacs_src
  dimacsmain.cpp
  utils.cpp
  basetree.cpp
  sankoff.cpp
  dimacs.cpp
  )

set( gamma_hdr
  utils.h
  basetree.h
  )

set( gamma_src
  utils.cpp
  basetree.cpp
  gammamain.cpp
  )

add_executable( sankoff ${sankoff_src} ${sankoff_hdr} )
target_link_libraries( sankoff ${CommonLibs} )

add_executable( sample_sankoff ${sample_sankoff_src} ${sample_sankoff_hdr} )
target_link_libraries( sample_sankoff ${CommonLibs} )

add_executable( dimacs ${dimacs_src} ${dimacs_hdr} )
target_link_libraries( dimacs ${CommonLibs} )

add_executable( gamma ${gamma_src} ${gamma_hdr} )
target_link_libraries( gamma ${CommonLibs} )

