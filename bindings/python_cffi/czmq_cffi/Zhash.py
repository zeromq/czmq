################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
import utils
from . import destructors
libczmq_destructors = destructors.lib

class Zhash(object):
    """
    generic type-free hash container (simple)
    """

    def __init__(self):
        """
        Create a new, empty hash container
        """
        p = utils.lib.zhash_new()
        if p == utils.ffi.NULL:
            raise MemoryError("Could not allocate person")

        # ffi.gc returns a copy of the cdata object which will have the
        # destructor called when the Python object is GC'd:
        # https://cffi.readthedocs.org/en/latest/using.html#ffi-interface
        self._p = utils.ffi.gc(p, libczmq_destructors.zhash_destroy_py)

    def insert(self, key, item):
        """
        Insert item into hash table with specified key and item.
        If key is already present returns -1 and leaves existing item unchanged
        Returns 0 on success.
        """
        return utils.lib.zhash_insert(self._p, utils.to_bytes(key), item._p)

    def update(self, key, item):
        """
        Update item into hash table with specified key and item.
        If key is already present, destroys old item and inserts new one.
        Use free_fn method to ensure deallocator is properly called on item.
        """
        return utils.lib.zhash_update(self._p, utils.to_bytes(key), item._p)

    def delete(self, key):
        """
        Remove an item specified by key from the hash table. If there was no such
        item, this function does nothing.
        """
        return utils.lib.zhash_delete(self._p, utils.to_bytes(key))

    def lookup(self, key):
        """
        Return the item at the specified key, or null
        """
        return utils.lib.zhash_lookup(self._p, utils.to_bytes(key))

    def rename(self, old_key, new_key):
        """
        Reindexes an item from an old key to a new key. If there was no such
        item, does nothing. Returns 0 if successful, else -1.
        """
        return utils.lib.zhash_rename(self._p, utils.to_bytes(old_key), utils.to_bytes(new_key))

    def freefn(self, key, free_fn):
        """
        Set a free function for the specified hash table item. When the item is
        destroyed, the free function, if any, is called on that item.
        Use this when hash items are dynamically allocated, to ensure that
        you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
        Returns the item, or NULL if there is no such item.
        """
        return utils.lib.zhash_freefn(self._p, utils.to_bytes(key), free_fn)

    def size(self):
        """
        Return the number of keys/items in the hash table
        """
        return utils.lib.zhash_size(self._p)

    def dup(self):
        """
        Make copy of hash table; if supplied table is null, returns null.
        Does not copy items themselves. Rebuilds new table so may be slow on
        very large tables. NOTE: only works with item values that are strings
        since there's no other way to know how to duplicate the item value.
        """
        return utils.lib.zhash_dup(self._p)

    def keys(self):
        """
        Return keys for items in table
        """
        return utils.lib.zhash_keys(self._p)

    def first(self):
        """
        Simple iterator; returns first item in hash table, in no given order,
        or NULL if the table is empty. This method is simpler to use than the
        foreach() method, which is deprecated. To access the key for this item
        use zhash_cursor(). NOTE: do NOT modify the table while iterating.
        """
        return utils.lib.zhash_first(self._p)

    def next(self):
        """
        Simple iterator; returns next item in hash table, in no given order,
        or NULL if the last item was already returned. Use this together with
        zhash_first() to process all items in a hash table. If you need the
        items in sorted order, use zhash_keys() and then zlist_sort(). To
        access the key for this item use zhash_cursor(). NOTE: do NOT modify
        the table while iterating.
        """
        return utils.lib.zhash_next(self._p)

    def cursor(self):
        """
        After a successful first/next method, returns the key for the item that
        was returned. This is a constant string that you may not modify or
        deallocate, and which lasts as long as the item in the hash. After an
        unsuccessful first/next, returns NULL.
        """
        return utils.lib.zhash_cursor(self._p)

    def comment(self, format, ):
        """
        Add a comment to hash table before saving to disk. You can add as many
        comment lines as you like. These comment lines are discarded when loading
        the file. If you use a null format, all comments are deleted.
        """
        return utils.lib.zhash_comment(self._p, format, )

    def pack(self):
        """
        Serialize hash table to a binary frame that can be sent in a message.
        The packed format is compatible with the 'dictionary' type defined in
        http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:

           ; A list of name/value pairs
           dictionary      = dict-count *( dict-name dict-value )
           dict-count      = number-4
           dict-value      = longstr
           dict-name       = string

           ; Strings are always length + text contents
           longstr         = number-4 *VCHAR
           string          = number-1 *VCHAR

           ; Numbers are unsigned integers in network byte order
           number-1        = 1OCTET
           number-4        = 4OCTET

        Comments are not included in the packed data. Item values MUST be
        strings.
        """
        return utils.lib.zhash_pack(self._p)

    def save(self, filename):
        """
        Save hash table to a text file in name=value format. Hash values must be
        printable strings; keys may not contain '=' character. Returns 0 if OK,
        else -1 if a file error occurred.
        """
        return utils.lib.zhash_save(self._p, utils.to_bytes(filename))

    def load(self, filename):
        """
        Load hash table from a text file in name=value format; hash table must
        already exist. Hash values must printable strings; keys may not contain
        '=' character. Returns 0 if OK, else -1 if a file was not readable.
        """
        return utils.lib.zhash_load(self._p, utils.to_bytes(filename))

    def refresh(self):
        """
        When a hash table was loaded from a file by zhash_load, this method will
        reload the file if it has been modified since, and is "stable", i.e. not
        still changing. Returns 0 if OK, -1 if there was an error reloading the
        file.
        """
        return utils.lib.zhash_refresh(self._p)

    def autofree(self):
        """
        Set hash for automatic value destruction. Note that this assumes that
        values are NULL-terminated strings. Do not use with different types.
        """
        return utils.lib.zhash_autofree(self._p)

    def test(verbose):
        """
        Self test of this class.
        """
        return utils.lib.zhash_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
