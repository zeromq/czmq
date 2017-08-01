#e -*- mode: ruby -*-
# vi: set ft=ruby :

# This will setup a clean Ubuntu1404 LTS env

$script = <<SCRIPT
apt-get update
apt-get install -y python-pip python-dev git htop virtualenvwrapper python2.7 python-virtualenv python-support cython \
git build-essential libtool pkg-config autotools-dev autoconf automake cmake uuid-dev libpcre3-dev valgrind \
libffi-dev

# only execute this next line if interested in updating the man pages as well (adds to build time):
# sudo apt-get install -y asciidoc

cd /home/vagrant
git clone --quiet https://github.com/zeromq/libzmq.git libzmq
cd /home/vagrant/libzmq
./autogen.sh
./configure
make
make install
ldconfig

cd /vagrant
./autogen.sh
./configure
make
make check
make install
ldconfig
SCRIPT

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"
VAGRANTFILE_LOCAL = 'Vagrantfile.local'

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = 'ubuntu/trusty64'
  config.vm.provision "shell", inline: $script

  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--cpus", "2", "--ioapic", "on", "--memory", "1024" ]
  end

  if File.file?(VAGRANTFILE_LOCAL)
    external = File.read VAGRANTFILE_LOCAL
    eval external
  end
end
