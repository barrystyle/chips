Chips integration/staging tree
==============================


What is CHIPS?
--------------

CHIPS is a digital crypto currency which is used across all the gaming platforms designed and developed using the pangea protocol. CHIPS is a  BTC fork with an apow(adoptive proof of work) integration with a block time adjusted to less than 10 seconds to suits to the needs of the betting in real time using CHIPS. Like BTC, CHIPS uses peer-to-peer technology to operate with no central authority: managing transactions and issuing money are carried out collectively by the network. CHIPS Core is the name of open source software which enables the use of this currency.

For more information, read the [original whitepaper](https://cdn.discordapp.com/attachments/455737840668770315/456036359870611457/Unsolicited_PANGEA_WP.pdf). <br/>
The first post about CHIPS by jl777 in [bitcointalk](https://bitcointalk.org/index.php?topic=2078449.0).


How do I build the software?
----------------------------

The most troublefree and reproducable method of building the repository is via the depends method:

    git clone https://github.com/barrystyle/chips
    cd chips/depends
    make HOST=x86_64-linux-gnu -j6
    cd ..
    ./autogen.sh
    CONFIG_SITE=$PWD/depends/x86_64-linux-gnu/share/config.site ./configure
    make

Each step must be done in order (particularly autogen.sh after depends).


What is Pangea Protocol?
------------------------

You can find more details and implementation of Pangea protocol in the [bet repo](https://github.com/chips-blockchain/bet.git). 
A fully dencentralized privacy preserving poker game is developed using the Pangea protocol and that uses CHIPS crypto currency for
real time betting and to play the game. The backend implementation of the poker game is been developing in the `bet repo` and front end 
development is happening in the [pangea-poker repo](https://github.com/chips-blockchain/pangea-poker).


CHIPS Community - Discord
-------------------------

We have an active [discord channel](https://discord.gg/tV7ADNE) where you can get to know more about CHIPS.

