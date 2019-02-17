# Have vagrant setup an Ubuntu 16.04 VM with 2GB RAM
Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/xenial64"
  config.vm.provider "virtualbox" do |vb|
    vb.memory = "2048"
  end
  config.vm.hostname = "ProtobuffZmqTest"

  # In the first run, have the VM setup the environment by
  # 1. Setting up the environment with all of the required dependencies to run this project
  # 2. Downloading and compiling protobuf (local-build since global build is too easy)  
  config.vm.provision "bootstrap",
    type: "shell",
    inline: <<-SHELL
	  echo -en "\t==============================================================\n"
	  echo -en "\t= Going to Build Dependencies for Project\n"
	  echo -en "\t==============================================================\n\n"
	  sleep 1
          sudo apt-get update
          sudo apt-get upgrade
          sudo apt-get install -yq git cmake autoconf automake libtool curl make g++ unzip pkg-config build-essential libzmq3-dev
      
	  clear
          echo -en "\t==============================================================\n"
          echo -en "\t= Performing a local build of Protobuf\n"
          echo -en "\t==============================================================\n"
 
	        # The user wants us to build Protobuf
		sleep 1

		echo -en "\n\t [1/4] Going to build protobuf. Downloading protobuf...\n"

                PROTOBUF_DIR="/home/vagrant/protobuf"
		mkdir $PROTOBUF_DIR 
		git clone https://github.com/protocolbuffers/protobuf.git $PROTOBUF_DIR
                cd $PROTOBUF_DIR
		git submodule update --init --recursive

		echo -en "\n\t [2/4] Compiling protobuf...\n"
		sleep 1
		./autogen.sh
		if [ $? ]
		then
			./configure 
			if [ $? ]
			then
				echo -en "\n\t [3/4] Making protobuf...\n"
				sleep 1
				make
				if [ $? ]
				then
					make install
                                        sudo ldconfig
					echo -en "\n\t [4/4] Done building protobuf...\n\n\n"
				else
					echo -en "\n\t Make failed in /vagrant/protobuf. Exiting...\n"
				fi
			else
				echo -en "\n\t Running /vagrant/protobuf/configure script failed. Exiting...\n"
			fi
		else
		    echo -en "\n\t Running /vagrant/protobuf/autogen.sh script failed, exiting...\n"
		fi
	  
	  echo -en "\n\t==============================================================\n"
	  echo -en "\t= Done Setting up Vagrant Environment\n"
	  echo -en "\t==============================================================\n\n\n"
	  
    SHELL

  # add the local user git config to the vm
  config.vm.provision "file", source: "~/.gitconfig", destination: ".gitconfig"


end
