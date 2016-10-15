import os
import sys

env = Environment(CXXFLAGS='-std=c++14 -O2',
                  LINKFLAGS='-O2',
                  CPPPATH=['../src'])

def add_path(env, path):
  env.Append(CPPPATH = [os.path.join(path, 'include')])
  env.Append(LIBPATH = [os.path.join(path, 'lib')])

if 'GTEST_PATH' in os.environ:
  add_path(env, os.environ['GTEST_PATH'])

if 'BOOST_PATH' in os.environ:
  add_path(env, os.environ['BOOST_PATH'])

if 'CXX' in os.environ:
  env.Replace(CXX = os.environ['CXX'])

def check_boost_lib(conf, name, header = None):
  """Checks a boost library.

  This method checks for the existance of a boost library and modifies the
  given Configure object accordingly. As some boost libraries can be
  configured to support multithreading, this method checks for regular and
  multithreaded-aware versions.

  Most libraries contain a header file in the top include directory with the
  same name as the library itself. In some cases, however, that is not the
  case. The optional parameter header can be used to provide a specific
  header file to look for.

  Args:
    conf: The Configure object.
    name: The library name.
    header: The header file to check for.
  """
  if header is None:
    header = '{}.hpp'.format(name)
  header = 'boost/{}'.format(header)

  if not conf.CheckLibWithHeader(
      'boost_{}-mt'.format(name), header, 'cxx'):
    if not conf.CheckLibWithHeader(
        'boost_{}', header, 'cxx'):
      print 'Could not find boost::{}'.format(name)
      Exit(1)

def configure(env):
  """Configures the environment."""
  conf = Configure(env)

  if not conf.CheckLib('boost_system', language='cxx'):
    print 'Could not find boost::system'
    Exit(1)

  boost_libs = [
      'atomic',
      'chrono',
      'date_time',
      'filesystem',
      'program_options',
      'regex',
      'thread'
  ]
  for lib in boost_libs:
    check_boost_lib(conf, lib)

  check_boost_lib(conf, 'log', 'log/trivial.hpp')

  if sys.platform == 'linux2' \
      and not conf.CheckLib('pthread'):
    print 'Could not find pthread'
    Exit(1)

  return conf.Finish()

def configure_test(env):
  """Configures the environment for the test target."""
  conf = Configure(env)
  if not conf.CheckLibWithHeader(
      'gtest', 'gtest/gtest.h', 'cxx'):
    print 'Could not find Google Test'
    Exit(1)
  return conf.Finish()

if not env.GetOption('clean'):
  env = configure(env)

if 'test' in COMMAND_LINE_TARGETS:
  test_env = env.Clone()
  test_env = configure_test(test_env)
  test_env.Replace(CXXFLAGS = '-std=c++14 -g -O0 --coverage')
  test_env.Replace(LINKFLAGS = '-g -O0 --coverage')
  SConscript('src/SConscript', variant_dir='test', duplicate=False, exports='test_env')
else:
  SConscript('src/SConscript', variant_dir='build', duplicate=False, exports='env')

