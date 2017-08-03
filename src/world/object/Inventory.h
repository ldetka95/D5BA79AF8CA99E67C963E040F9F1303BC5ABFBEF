#pragma once

#define INVENTORY_CLASS 12564
#define INVENTORY_ITEMINFO_CLASS 12565

#include "../weapon/Weapon.h"

class Inventory : public Restorable {
    private:

    class ItemInfo : public Restorable {
        public:
        ItemInfo() : Restorable( INVENTORY_ITEMINFO_CLASS ) {
            _id = 0;
            _wpn = NULL;
        }
        ~ItemInfo() {
        }
        ID getID() {
            return _id;
        }
        void setID( ID id ) {
            _id = id;
        }
        Weapon* getWeapon() {
            return _wpn;
        }
        void setWeapon( Weapon* wpn ) {
            _wpn = wpn;
        }

        bool onSerialize( AbstractSerializationOutputStream* stream ) {
            return true;
        }

        bool onDeserialize( AbstractSerializationInputStream* stream ) {
            return true;
        }

        private:

        ID _id;
        Weapon* _wpn;

    };


    class Builder : public RestorableBuilder {
        public:
        Builder() : RestorableBuilder() {}
        ~Builder() {}
        Inventory* build() {
            return new Inventory();
        }
    };

    class ItemInfoBuilder : public RestorableBuilder {
        public:
        ItemInfoBuilder() : RestorableBuilder() {}
        ~ItemInfoBuilder() {}
        ItemInfo* build() {
            return new ItemInfo();
        }
    };


    public:
    Inventory() : Restorable( INVENTORY_CLASS ) {
        _selectedWeapon = NULL;
        _selectedItem = ___items.begin();
    }
    ~Inventory() {
        map < ID, ItemInfo* >::iterator it = ___items.begin();
        while ( it != ___items.end() ) {
            delete it -> second;
            it++;
        }
    }

    static void Register() {
        Restorable::Register( INVENTORY_CLASS, new Builder() );
        Restorable::Register( INVENTORY_ITEMINFO_CLASS, new ItemInfoBuilder() );
    }

    bool onSerialize( AbstractSerializationOutputStream* stream ) {
        return true;
    }

    bool onDeserialize( AbstractSerializationInputStream* stream ) {
        return true;
    }

    void addWeapon( ID id, Weapon* wpn ) {
        ItemInfo* info = new ItemInfo();
        info -> setID( id );
        info -> setWeapon( wpn );
        ___items[ id ] = info;
    }

    void selectWeapon( ID id ) {
        map < ID, ItemInfo* >::iterator found = ___items.find( id );
        if ( found != ___items.end() ) {
            _selectedWeapon = found -> second;
            _selectedItem = found;
        }
    }

    Weapon* getSelectedWeapon() {
        if ( _selectedWeapon ) {
            return _selectedWeapon -> getWeapon();
        }
        return NULL;
    }

    ID getSelectedWeaponID() {
        if ( _selectedWeapon ) {
            return _selectedWeapon -> getID();
        }
        return -1;
    }

    void nextWeapon() {
        if ( _selectedItem != ___items.end() ) {
            map < ID, ItemInfo* >::iterator it = _selectedItem;
            it++;
            if ( it != ___items.end() ) {
                _selectedItem = it;
            } else {
                _selectedItem = ___items.begin();
            }
        }
    }

    void prevWeapon() {
        if ( _selectedItem != ___items.begin() ) {
            _selectedItem--;
        } else {
            map < ID, ItemInfo* >::reverse_iterator found = ___items.rbegin();
            ID id = found -> first;
            _selectedItem = ___items.find( id );
        }
    }

    private:

    map < ID, ItemInfo* > ___items;
    map < ID, ItemInfo* >::iterator _selectedItem;
    ItemInfo* _selectedWeapon;

};


