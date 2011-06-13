#-
# Copyright (c) 2011 Daemotron <mail@daemotron.net>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

include_directories(${libsynctory_SOURCE_DIR}/src/include)
link_directories(${libsynctory_BINARY_DIR}/src/lib)


# Add your tests to this list
set(
    SYNCTORY_TEST_CASES
    test_version.c
    test_fingerprint.c
    test_diff.c
)


set(
    SYNCTORY_TEST_MISC
    main.c
    tests.c
    helpers.c
)


add_executable(synctory_test ${SYNCTORY_TEST_MISC} ${SYNCTORY_TEST_CASES})
target_link_libraries(synctory_test synctory)


#
# the following tests are broken, deprecated and will be removed on next occasion
#

#add_subdirectory(checksum)
#add_subdirectory(fingerprint)
#add_subdirectory(diff)
#add_subdirectory(synth)