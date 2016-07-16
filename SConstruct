import os.path

# TODO use env variables
GTEST_PATH = '/Users/vjimenez/soft/googletest/googletest/install'
BOOST_PATH = '/usr/local/Cellar/boost/1.60.0_2'

env = Environment(CXXFLAGS='-std=c++14 -O2',
                  CPPPATH=['src',
                           os.path.join(GTEST_PATH, 'include'),
                           os.path.join(BOOST_PATH, 'include')],
                  LIBPATH=[os.path.join(GTEST_PATH, 'lib'),
                           os.path.join(BOOST_PATH, 'lib')])

if not env.GetOption('clean'):
  conf = Configure(env)
  if not conf.CheckLibWithHeader(
      'gtest', 'gtest/gtest.h', 'cxx'):
    print 'Could not find Google Test'
    Exit(1)
  if not conf.CheckLib('boost_system'):
    print 'Could not find boost::system'
    Exit(1)
  if not conf.CheckLibWithHeader(
      'boost_filesystem', 'boost/filesystem.hpp', 'cxx'):
    print 'Could not find boost::filesystem'
    Exit(1)
  env = conf.Finish()

if 'test' in COMMAND_LINE_TARGETS:
  test_env = env.Clone()
  test_env.Append(CPPPATH = ['../src'])
  SConscript('src/SConscript', variant_dir='test', duplicate=False, exports='test_env')
else:
  SConscript('src/SConscript', variant_dir='build', duplicate=False, exports='env')

