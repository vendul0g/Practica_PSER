#!/bin/bash

sudo apt-get update && 
sudo apt-get install apt-transport-https ca-certificates curl software-properties-common -y &&
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add - &&
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" &&
sudo apt-get update &&
sudo apt-get install docker-ce -y &&
sudo apt-get install docker-compose -y &&
#add user to docker group
sudo usermod -aG docker $USER &&
echo "User $USER added to docker group" &&
echo "You will need to log out and log in before accessing docker as regular user"

