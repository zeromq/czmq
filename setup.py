from distutils.core import setup

setup(name='czmq',
	  description="""The high-level C binding for 0MQ""",
	  version='0.1',
	  url='https://github.com/zeromq/czmq',
	  packages=['czmq'],
	  package_dir={'': 'bindings/python'},
)
