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

def configure(env):
  conf = Configure(env)

  if not conf.CheckLib('boost_system', language='cxx'):
    print 'Could not find boost::system'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_filesystem', 'boost/filesystem.hpp', 'cxx'):
    print 'Could not find boost::filesystem'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_atomic', 'boost/atomic.hpp', 'cxx'):
    print 'Could not find boost::atomic'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_chrono', 'boost/chrono.hpp', 'cxx'):
    print 'Could not find boost::chrono'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_thread', 'boost/thread.hpp', 'cxx'):
    print 'Could not find boost::thread'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_date_time', 'boost/date_time.hpp', 'cxx'):
    print 'Could not find boost::date_time'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_regex', 'boost/regex.hpp', 'cxx'):
    print 'Could not find boost::regex'
    Exit(1)

  if not conf.CheckLibWithHeader(
      'boost_program_options', 'boost/program_options.hpp', 'cxx'):
    print 'Could not find boost::program_options'
    Exit(1)

  # check for either boost-log or boost-log-mt
  if not conf.CheckLibWithHeader(
      'boost_log-mt', 'boost/log/trivial.hpp', 'cxx'):
    if not conf.CheckLibWithHeader(
        'boost_log', 'boost/log/trivial.hpp', 'cxx'):
      print 'Could not find boost::log'
      Exit(1)

  if sys.platform == 'linux2' \
      and not conf.CheckLib('pthread'):
    print 'Could not find pthread'
    Exit(1)

  return conf.Finish()

def configure_test(env):
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

