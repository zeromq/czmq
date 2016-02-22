/*  =========================================================================
    czmq Node.js binding

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#define CZMQ_BUILD_DRAFT_API
#include "czmq.h.h"
#include "nan.h"

using namespace v8;
using namespace Nan;

class Zactor: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zactor").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "is", is);
            Nan::SetPrototypeMethod (tpl, "resolve", resolve);
            Nan::SetPrototypeMethod (tpl, "sock", sock);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zactor").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zactor (arguments) {
            self = new (name);
        }
        ~Zactor () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zactor *zactor;
        zactor = new Zactor ();
        if (zactor) {
            zactor->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_destroy (&zactor->self);
    }

    static NAN_METHOD (defined) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zactor->self != NULL));
    }

    static NAN_METHOD (send) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_send (zactor->self);
    }

    static NAN_METHOD (recv) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_recv (zactor->self);
    }

    static NAN_METHOD (is) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_is (zactor->self);
    }

    static NAN_METHOD (resolve) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_resolve (zactor->self);
    }

    static NAN_METHOD (sock) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_sock (zactor->self);
    }

    static NAN_METHOD (test) {
        Zactor *zactor = Nan::ObjectWrap::Unwrap <Zactor> (info.Holder ());
        zactor_test (zactor->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zactor_t *self;
    public:
        zactor_t *get_self () {
            return self;
        }
};

class Zarmour: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zarmour").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "encode", encode);
            Nan::SetPrototypeMethod (tpl, "decode", decode);
            Nan::SetPrototypeMethod (tpl, "mode", mode);
            Nan::SetPrototypeMethod (tpl, "modeStr", mode_str);
            Nan::SetPrototypeMethod (tpl, "setMode", set_mode);
            Nan::SetPrototypeMethod (tpl, "pad", pad);
            Nan::SetPrototypeMethod (tpl, "setPad", set_pad);
            Nan::SetPrototypeMethod (tpl, "padChar", pad_char);
            Nan::SetPrototypeMethod (tpl, "setPadChar", set_pad_char);
            Nan::SetPrototypeMethod (tpl, "lineBreaks", line_breaks);
            Nan::SetPrototypeMethod (tpl, "setLineBreaks", set_line_breaks);
            Nan::SetPrototypeMethod (tpl, "lineLength", line_length);
            Nan::SetPrototypeMethod (tpl, "setLineLength", set_line_length);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zarmour").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zarmour (arguments) {
            self = new (name);
        }
        ~Zarmour () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zarmour *zarmour;
        zarmour = new Zarmour ();
        if (zarmour) {
            zarmour->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_destroy (&zarmour->self);
    }

    static NAN_METHOD (defined) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zarmour->self != NULL));
    }

    static NAN_METHOD (encode) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_encode (zarmour->self);
    }

    static NAN_METHOD (decode) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_decode (zarmour->self);
    }

    static NAN_METHOD (mode) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_mode (zarmour->self);
    }

    static NAN_METHOD (mode_str) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_mode_str (zarmour->self);
    }

    static NAN_METHOD (set_mode) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_set_mode (zarmour->self);
    }

    static NAN_METHOD (pad) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_pad (zarmour->self);
    }

    static NAN_METHOD (set_pad) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_set_pad (zarmour->self);
    }

    static NAN_METHOD (pad_char) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_pad_char (zarmour->self);
    }

    static NAN_METHOD (set_pad_char) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_set_pad_char (zarmour->self);
    }

    static NAN_METHOD (line_breaks) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_line_breaks (zarmour->self);
    }

    static NAN_METHOD (set_line_breaks) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_set_line_breaks (zarmour->self);
    }

    static NAN_METHOD (line_length) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_line_length (zarmour->self);
    }

    static NAN_METHOD (set_line_length) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_set_line_length (zarmour->self);
    }

    static NAN_METHOD (print) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_print (zarmour->self);
    }

    static NAN_METHOD (test) {
        Zarmour *zarmour = Nan::ObjectWrap::Unwrap <Zarmour> (info.Holder ());
        zarmour_test (zarmour->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zarmour_t *self;
    public:
        zarmour_t *get_self () {
            return self;
        }
};

class Zcert: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zcert").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "publicKey", public_key);
            Nan::SetPrototypeMethod (tpl, "secretKey", secret_key);
            Nan::SetPrototypeMethod (tpl, "publicTxt", public_txt);
            Nan::SetPrototypeMethod (tpl, "secretTxt", secret_txt);
            Nan::SetPrototypeMethod (tpl, "setMeta", set_meta);
            Nan::SetPrototypeMethod (tpl, "unsetMeta", unset_meta);
            Nan::SetPrototypeMethod (tpl, "meta", meta);
            Nan::SetPrototypeMethod (tpl, "metaKeys", meta_keys);
            Nan::SetPrototypeMethod (tpl, "save", save);
            Nan::SetPrototypeMethod (tpl, "savePublic", save_public);
            Nan::SetPrototypeMethod (tpl, "saveSecret", save_secret);
            Nan::SetPrototypeMethod (tpl, "apply", apply);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "eq", eq);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "fprint", fprint);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zcert").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zcert (arguments) {
            self = new (name);
        }
        ~Zcert () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zcert *zcert;
        zcert = new Zcert ();
        if (zcert) {
            zcert->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_destroy (&zcert->self);
    }

    static NAN_METHOD (defined) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zcert->self != NULL));
    }

    static NAN_METHOD (public_key) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_public_key (zcert->self);
    }

    static NAN_METHOD (secret_key) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_secret_key (zcert->self);
    }

    static NAN_METHOD (public_txt) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_public_txt (zcert->self);
    }

    static NAN_METHOD (secret_txt) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_secret_txt (zcert->self);
    }

    static NAN_METHOD (set_meta) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_set_meta (zcert->self);
    }

    static NAN_METHOD (unset_meta) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_unset_meta (zcert->self);
    }

    static NAN_METHOD (meta) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_meta (zcert->self);
    }

    static NAN_METHOD (meta_keys) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_meta_keys (zcert->self);
    }

    static NAN_METHOD (save) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_save (zcert->self);
    }

    static NAN_METHOD (save_public) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_save_public (zcert->self);
    }

    static NAN_METHOD (save_secret) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_save_secret (zcert->self);
    }

    static NAN_METHOD (apply) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_apply (zcert->self);
    }

    static NAN_METHOD (dup) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_dup (zcert->self);
    }

    static NAN_METHOD (eq) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_eq (zcert->self);
    }

    static NAN_METHOD (print) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_print (zcert->self);
    }

    static NAN_METHOD (fprint) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_fprint (zcert->self);
    }

    static NAN_METHOD (test) {
        Zcert *zcert = Nan::ObjectWrap::Unwrap <Zcert> (info.Holder ());
        zcert_test (zcert->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zcert_t *self;
    public:
        zcert_t *get_self () {
            return self;
        }
};

class Zcertstore: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zcertstore").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "lookup", lookup);
            Nan::SetPrototypeMethod (tpl, "insert", insert);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "fprint", fprint);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zcertstore").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zcertstore (arguments) {
            self = new (name);
        }
        ~Zcertstore () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zcertstore *zcertstore;
        zcertstore = new Zcertstore ();
        if (zcertstore) {
            zcertstore->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_destroy (&zcertstore->self);
    }

    static NAN_METHOD (defined) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zcertstore->self != NULL));
    }

    static NAN_METHOD (lookup) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_lookup (zcertstore->self);
    }

    static NAN_METHOD (insert) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_insert (zcertstore->self);
    }

    static NAN_METHOD (print) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_print (zcertstore->self);
    }

    static NAN_METHOD (fprint) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_fprint (zcertstore->self);
    }

    static NAN_METHOD (test) {
        Zcertstore *zcertstore = Nan::ObjectWrap::Unwrap <Zcertstore> (info.Holder ());
        zcertstore_test (zcertstore->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zcertstore_t *self;
    public:
        zcertstore_t *get_self () {
            return self;
        }
};

class Zchunk: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zchunk").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "resize", resize);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "maxSize", max_size);
            Nan::SetPrototypeMethod (tpl, "data", data);
            Nan::SetPrototypeMethod (tpl, "set", set);
            Nan::SetPrototypeMethod (tpl, "fill", fill);
            Nan::SetPrototypeMethod (tpl, "append", append);
            Nan::SetPrototypeMethod (tpl, "extend", extend);
            Nan::SetPrototypeMethod (tpl, "consume", consume);
            Nan::SetPrototypeMethod (tpl, "exhausted", exhausted);
            Nan::SetPrototypeMethod (tpl, "read", read);
            Nan::SetPrototypeMethod (tpl, "write", write);
            Nan::SetPrototypeMethod (tpl, "slurp", slurp);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "strhex", strhex);
            Nan::SetPrototypeMethod (tpl, "strdup", strdup);
            Nan::SetPrototypeMethod (tpl, "streq", streq);
            Nan::SetPrototypeMethod (tpl, "pack", pack);
            Nan::SetPrototypeMethod (tpl, "unpack", unpack);
            Nan::SetPrototypeMethod (tpl, "digest", digest);
            Nan::SetPrototypeMethod (tpl, "fprint", fprint);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "is", is);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zchunk").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zchunk (arguments) {
            self = new (name);
        }
        ~Zchunk () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zchunk *zchunk;
        zchunk = new Zchunk ();
        if (zchunk) {
            zchunk->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_destroy (&zchunk->self);
    }

    static NAN_METHOD (defined) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zchunk->self != NULL));
    }

    static NAN_METHOD (resize) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_resize (zchunk->self);
    }

    static NAN_METHOD (size) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_size (zchunk->self);
    }

    static NAN_METHOD (max_size) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_max_size (zchunk->self);
    }

    static NAN_METHOD (data) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_data (zchunk->self);
    }

    static NAN_METHOD (set) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_set (zchunk->self);
    }

    static NAN_METHOD (fill) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_fill (zchunk->self);
    }

    static NAN_METHOD (append) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_append (zchunk->self);
    }

    static NAN_METHOD (extend) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_extend (zchunk->self);
    }

    static NAN_METHOD (consume) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_consume (zchunk->self);
    }

    static NAN_METHOD (exhausted) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_exhausted (zchunk->self);
    }

    static NAN_METHOD (read) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_read (zchunk->self);
    }

    static NAN_METHOD (write) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_write (zchunk->self);
    }

    static NAN_METHOD (slurp) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_slurp (zchunk->self);
    }

    static NAN_METHOD (dup) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_dup (zchunk->self);
    }

    static NAN_METHOD (strhex) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_strhex (zchunk->self);
    }

    static NAN_METHOD (strdup) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_strdup (zchunk->self);
    }

    static NAN_METHOD (streq) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_streq (zchunk->self);
    }

    static NAN_METHOD (pack) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_pack (zchunk->self);
    }

    static NAN_METHOD (unpack) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_unpack (zchunk->self);
    }

    static NAN_METHOD (digest) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_digest (zchunk->self);
    }

    static NAN_METHOD (fprint) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_fprint (zchunk->self);
    }

    static NAN_METHOD (print) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_print (zchunk->self);
    }

    static NAN_METHOD (is) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_is (zchunk->self);
    }

    static NAN_METHOD (test) {
        Zchunk *zchunk = Nan::ObjectWrap::Unwrap <Zchunk> (info.Holder ());
        zchunk_test (zchunk->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zchunk_t *self;
    public:
        zchunk_t *get_self () {
            return self;
        }
};

class Zclock: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zclock").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "sleep", sleep);
            Nan::SetPrototypeMethod (tpl, "time", time);
            Nan::SetPrototypeMethod (tpl, "mono", mono);
            Nan::SetPrototypeMethod (tpl, "usecs", usecs);
            Nan::SetPrototypeMethod (tpl, "timestr", timestr);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zclock").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zclock () {
        }
        ~Zclock () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zclock *zclock;
        zclock = new Zclock ();
        if (zclock) {
            zclock->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (sleep) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_sleep (zclock->self);
    }

    static NAN_METHOD (time) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_time (zclock->self);
    }

    static NAN_METHOD (mono) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_mono (zclock->self);
    }

    static NAN_METHOD (usecs) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_usecs (zclock->self);
    }

    static NAN_METHOD (timestr) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_timestr (zclock->self);
    }

    static NAN_METHOD (test) {
        Zclock *zclock = Nan::ObjectWrap::Unwrap <Zclock> (info.Holder ());
        zclock_test (zclock->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zconfig: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zconfig").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "name", name);
            Nan::SetPrototypeMethod (tpl, "value", value);
            Nan::SetPrototypeMethod (tpl, "put", put);
            Nan::SetPrototypeMethod (tpl, "putf", putf);
            Nan::SetPrototypeMethod (tpl, "get", get);
            Nan::SetPrototypeMethod (tpl, "setName", set_name);
            Nan::SetPrototypeMethod (tpl, "setValue", set_value);
            Nan::SetPrototypeMethod (tpl, "child", child);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "locate", locate);
            Nan::SetPrototypeMethod (tpl, "atDepth", at_depth);
            Nan::SetPrototypeMethod (tpl, "execute", execute);
            Nan::SetPrototypeMethod (tpl, "setComment", set_comment);
            Nan::SetPrototypeMethod (tpl, "comments", comments);
            Nan::SetPrototypeMethod (tpl, "save", save);
            Nan::SetPrototypeMethod (tpl, "savef", savef);
            Nan::SetPrototypeMethod (tpl, "filename", filename);
            Nan::SetPrototypeMethod (tpl, "reload", reload);
            Nan::SetPrototypeMethod (tpl, "chunkLoad", chunk_load);
            Nan::SetPrototypeMethod (tpl, "chunkSave", chunk_save);
            Nan::SetPrototypeMethod (tpl, "strLoad", str_load);
            Nan::SetPrototypeMethod (tpl, "strSave", str_save);
            Nan::SetPrototypeMethod (tpl, "hasChanged", has_changed);
            Nan::SetPrototypeMethod (tpl, "fprint", fprint);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zconfig").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zconfig (arguments) {
            self = new (name);
        }
        ~Zconfig () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zconfig *zconfig;
        zconfig = new Zconfig ();
        if (zconfig) {
            zconfig->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_destroy (&zconfig->self);
    }

    static NAN_METHOD (defined) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zconfig->self != NULL));
    }

    static NAN_METHOD (name) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_name (zconfig->self);
    }

    static NAN_METHOD (value) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_value (zconfig->self);
    }

    static NAN_METHOD (put) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_put (zconfig->self);
    }

    static NAN_METHOD (putf) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_putf (zconfig->self);
    }

    static NAN_METHOD (get) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_get (zconfig->self);
    }

    static NAN_METHOD (set_name) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_set_name (zconfig->self);
    }

    static NAN_METHOD (set_value) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_set_value (zconfig->self);
    }

    static NAN_METHOD (child) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_child (zconfig->self);
    }

    static NAN_METHOD (next) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_next (zconfig->self);
    }

    static NAN_METHOD (locate) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_locate (zconfig->self);
    }

    static NAN_METHOD (at_depth) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_at_depth (zconfig->self);
    }

    static NAN_METHOD (execute) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_execute (zconfig->self);
    }

    static NAN_METHOD (set_comment) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_set_comment (zconfig->self);
    }

    static NAN_METHOD (comments) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_comments (zconfig->self);
    }

    static NAN_METHOD (save) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_save (zconfig->self);
    }

    static NAN_METHOD (savef) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_savef (zconfig->self);
    }

    static NAN_METHOD (filename) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_filename (zconfig->self);
    }

    static NAN_METHOD (reload) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_reload (zconfig->self);
    }

    static NAN_METHOD (chunk_load) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_chunk_load (zconfig->self);
    }

    static NAN_METHOD (chunk_save) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_chunk_save (zconfig->self);
    }

    static NAN_METHOD (str_load) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_str_load (zconfig->self);
    }

    static NAN_METHOD (str_save) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_str_save (zconfig->self);
    }

    static NAN_METHOD (has_changed) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_has_changed (zconfig->self);
    }

    static NAN_METHOD (fprint) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_fprint (zconfig->self);
    }

    static NAN_METHOD (print) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_print (zconfig->self);
    }

    static NAN_METHOD (test) {
        Zconfig *zconfig = Nan::ObjectWrap::Unwrap <Zconfig> (info.Holder ());
        zconfig_test (zconfig->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zconfig_t *self;
    public:
        zconfig_t *get_self () {
            return self;
        }
};

class Zdigest: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zdigest").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "update", update);
            Nan::SetPrototypeMethod (tpl, "data", data);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "string", string);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zdigest").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zdigest (arguments) {
            self = new (name);
        }
        ~Zdigest () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zdigest *zdigest;
        zdigest = new Zdigest ();
        if (zdigest) {
            zdigest->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_destroy (&zdigest->self);
    }

    static NAN_METHOD (defined) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zdigest->self != NULL));
    }

    static NAN_METHOD (update) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_update (zdigest->self);
    }

    static NAN_METHOD (data) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_data (zdigest->self);
    }

    static NAN_METHOD (size) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_size (zdigest->self);
    }

    static NAN_METHOD (string) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_string (zdigest->self);
    }

    static NAN_METHOD (test) {
        Zdigest *zdigest = Nan::ObjectWrap::Unwrap <Zdigest> (info.Holder ());
        zdigest_test (zdigest->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zdigest_t *self;
    public:
        zdigest_t *get_self () {
            return self;
        }
};

class Zdir: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zdir").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "path", path);
            Nan::SetPrototypeMethod (tpl, "modified", modified);
            Nan::SetPrototypeMethod (tpl, "cursize", cursize);
            Nan::SetPrototypeMethod (tpl, "count", count);
            Nan::SetPrototypeMethod (tpl, "list", list);
            Nan::SetPrototypeMethod (tpl, "remove", remove);
            Nan::SetPrototypeMethod (tpl, "diff", diff);
            Nan::SetPrototypeMethod (tpl, "resync", resync);
            Nan::SetPrototypeMethod (tpl, "cache", cache);
            Nan::SetPrototypeMethod (tpl, "fprint", fprint);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "watch", watch);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zdir").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zdir (arguments) {
            self = new (name);
        }
        ~Zdir () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zdir *zdir;
        zdir = new Zdir ();
        if (zdir) {
            zdir->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_destroy (&zdir->self);
    }

    static NAN_METHOD (defined) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zdir->self != NULL));
    }

    static NAN_METHOD (path) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_path (zdir->self);
    }

    static NAN_METHOD (modified) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_modified (zdir->self);
    }

    static NAN_METHOD (cursize) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_cursize (zdir->self);
    }

    static NAN_METHOD (count) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_count (zdir->self);
    }

    static NAN_METHOD (list) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_list (zdir->self);
    }

    static NAN_METHOD (remove) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_remove (zdir->self);
    }

    static NAN_METHOD (diff) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_diff (zdir->self);
    }

    static NAN_METHOD (resync) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_resync (zdir->self);
    }

    static NAN_METHOD (cache) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_cache (zdir->self);
    }

    static NAN_METHOD (fprint) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_fprint (zdir->self);
    }

    static NAN_METHOD (print) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_print (zdir->self);
    }

    static NAN_METHOD (watch) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_watch (zdir->self);
    }

    static NAN_METHOD (test) {
        Zdir *zdir = Nan::ObjectWrap::Unwrap <Zdir> (info.Holder ());
        zdir_test (zdir->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zdir_t *self;
    public:
        zdir_t *get_self () {
            return self;
        }
};

class ZdirPatch: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("ZdirPatch").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "path", path);
            Nan::SetPrototypeMethod (tpl, "file", file);
            Nan::SetPrototypeMethod (tpl, "op", op);
            Nan::SetPrototypeMethod (tpl, "vpath", vpath);
            Nan::SetPrototypeMethod (tpl, "digestSet", digest_set);
            Nan::SetPrototypeMethod (tpl, "digest", digest);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("ZdirPatch").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit ZdirPatch (arguments) {
            self = new (name);
        }
        ~ZdirPatch () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        ZdirPatch *zdir_patch;
        zdir_patch = new ZdirPatch ();
        if (zdir_patch) {
            zdir_patch->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_destroy (&zdir_patch->self);
    }

    static NAN_METHOD (defined) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zdir_patch->self != NULL));
    }

    static NAN_METHOD (dup) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_dup (zdir_patch->self);
    }

    static NAN_METHOD (path) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_path (zdir_patch->self);
    }

    static NAN_METHOD (file) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_file (zdir_patch->self);
    }

    static NAN_METHOD (op) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_op (zdir_patch->self);
    }

    static NAN_METHOD (vpath) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_vpath (zdir_patch->self);
    }

    static NAN_METHOD (digest_set) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_digest_set (zdir_patch->self);
    }

    static NAN_METHOD (digest) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_digest (zdir_patch->self);
    }

    static NAN_METHOD (test) {
        ZdirPatch *zdir_patch = Nan::ObjectWrap::Unwrap <ZdirPatch> (info.Holder ());
        zdir_patch_test (zdir_patch->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zdir_patch_t *self;
    public:
        zdir_patch_t *get_self () {
            return self;
        }
};

class Zfile: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zfile").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "filename", filename);
            Nan::SetPrototypeMethod (tpl, "restat", restat);
            Nan::SetPrototypeMethod (tpl, "modified", modified);
            Nan::SetPrototypeMethod (tpl, "cursize", cursize);
            Nan::SetPrototypeMethod (tpl, "isDirectory", is_directory);
            Nan::SetPrototypeMethod (tpl, "isRegular", is_regular);
            Nan::SetPrototypeMethod (tpl, "isReadable", is_readable);
            Nan::SetPrototypeMethod (tpl, "isWriteable", is_writeable);
            Nan::SetPrototypeMethod (tpl, "isStable", is_stable);
            Nan::SetPrototypeMethod (tpl, "hasChanged", has_changed);
            Nan::SetPrototypeMethod (tpl, "remove", remove);
            Nan::SetPrototypeMethod (tpl, "input", input);
            Nan::SetPrototypeMethod (tpl, "output", output);
            Nan::SetPrototypeMethod (tpl, "read", read);
            Nan::SetPrototypeMethod (tpl, "eof", eof);
            Nan::SetPrototypeMethod (tpl, "write", write);
            Nan::SetPrototypeMethod (tpl, "readln", readln);
            Nan::SetPrototypeMethod (tpl, "close", close);
            Nan::SetPrototypeMethod (tpl, "handle", handle);
            Nan::SetPrototypeMethod (tpl, "digest", digest);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zfile").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zfile (arguments) {
            self = new (name);
        }
        ~Zfile () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zfile *zfile;
        zfile = new Zfile ();
        if (zfile) {
            zfile->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_destroy (&zfile->self);
    }

    static NAN_METHOD (defined) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zfile->self != NULL));
    }

    static NAN_METHOD (dup) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_dup (zfile->self);
    }

    static NAN_METHOD (filename) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_filename (zfile->self);
    }

    static NAN_METHOD (restat) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_restat (zfile->self);
    }

    static NAN_METHOD (modified) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_modified (zfile->self);
    }

    static NAN_METHOD (cursize) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_cursize (zfile->self);
    }

    static NAN_METHOD (is_directory) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_is_directory (zfile->self);
    }

    static NAN_METHOD (is_regular) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_is_regular (zfile->self);
    }

    static NAN_METHOD (is_readable) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_is_readable (zfile->self);
    }

    static NAN_METHOD (is_writeable) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_is_writeable (zfile->self);
    }

    static NAN_METHOD (is_stable) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_is_stable (zfile->self);
    }

    static NAN_METHOD (has_changed) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_has_changed (zfile->self);
    }

    static NAN_METHOD (remove) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_remove (zfile->self);
    }

    static NAN_METHOD (input) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_input (zfile->self);
    }

    static NAN_METHOD (output) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_output (zfile->self);
    }

    static NAN_METHOD (read) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_read (zfile->self);
    }

    static NAN_METHOD (eof) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_eof (zfile->self);
    }

    static NAN_METHOD (write) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_write (zfile->self);
    }

    static NAN_METHOD (readln) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_readln (zfile->self);
    }

    static NAN_METHOD (close) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_close (zfile->self);
    }

    static NAN_METHOD (handle) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_handle (zfile->self);
    }

    static NAN_METHOD (digest) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_digest (zfile->self);
    }

    static NAN_METHOD (test) {
        Zfile *zfile = Nan::ObjectWrap::Unwrap <Zfile> (info.Holder ());
        zfile_test (zfile->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zfile_t *self;
    public:
        zfile_t *get_self () {
            return self;
        }
};

class Zframe: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zframe").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "data", data);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "strhex", strhex);
            Nan::SetPrototypeMethod (tpl, "strdup", strdup);
            Nan::SetPrototypeMethod (tpl, "streq", streq);
            Nan::SetPrototypeMethod (tpl, "more", more);
            Nan::SetPrototypeMethod (tpl, "setMore", set_more);
            Nan::SetPrototypeMethod (tpl, "routingId", routing_id);
            Nan::SetPrototypeMethod (tpl, "setRoutingId", set_routing_id);
            Nan::SetPrototypeMethod (tpl, "group", group);
            Nan::SetPrototypeMethod (tpl, "setGroup", set_group);
            Nan::SetPrototypeMethod (tpl, "eq", eq);
            Nan::SetPrototypeMethod (tpl, "reset", reset);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "is", is);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zframe").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zframe (arguments) {
            self = new (name);
        }
        ~Zframe () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zframe *zframe;
        zframe = new Zframe ();
        if (zframe) {
            zframe->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_destroy (&zframe->self);
    }

    static NAN_METHOD (defined) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zframe->self != NULL));
    }

    static NAN_METHOD (send) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_send (zframe->self);
    }

    static NAN_METHOD (size) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_size (zframe->self);
    }

    static NAN_METHOD (data) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_data (zframe->self);
    }

    static NAN_METHOD (dup) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_dup (zframe->self);
    }

    static NAN_METHOD (strhex) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_strhex (zframe->self);
    }

    static NAN_METHOD (strdup) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_strdup (zframe->self);
    }

    static NAN_METHOD (streq) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_streq (zframe->self);
    }

    static NAN_METHOD (more) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_more (zframe->self);
    }

    static NAN_METHOD (set_more) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_set_more (zframe->self);
    }

    static NAN_METHOD (routing_id) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_routing_id (zframe->self);
    }

    static NAN_METHOD (set_routing_id) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_set_routing_id (zframe->self);
    }

    static NAN_METHOD (group) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_group (zframe->self);
    }

    static NAN_METHOD (set_group) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_set_group (zframe->self);
    }

    static NAN_METHOD (eq) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_eq (zframe->self);
    }

    static NAN_METHOD (reset) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_reset (zframe->self);
    }

    static NAN_METHOD (print) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_print (zframe->self);
    }

    static NAN_METHOD (is) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_is (zframe->self);
    }

    static NAN_METHOD (test) {
        Zframe *zframe = Nan::ObjectWrap::Unwrap <Zframe> (info.Holder ());
        zframe_test (zframe->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zframe_t *self;
    public:
        zframe_t *get_self () {
            return self;
        }
};

class Zhash: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zhash").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "insert", insert);
            Nan::SetPrototypeMethod (tpl, "update", update);
            Nan::SetPrototypeMethod (tpl, "delete", delete);
            Nan::SetPrototypeMethod (tpl, "lookup", lookup);
            Nan::SetPrototypeMethod (tpl, "rename", rename);
            Nan::SetPrototypeMethod (tpl, "freefn", freefn);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "keys", keys);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "cursor", cursor);
            Nan::SetPrototypeMethod (tpl, "comment", comment);
            Nan::SetPrototypeMethod (tpl, "pack", pack);
            Nan::SetPrototypeMethod (tpl, "save", save);
            Nan::SetPrototypeMethod (tpl, "load", load);
            Nan::SetPrototypeMethod (tpl, "refresh", refresh);
            Nan::SetPrototypeMethod (tpl, "autofree", autofree);
            Nan::SetPrototypeMethod (tpl, "foreach", foreach);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zhash").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zhash (arguments) {
            self = new (name);
        }
        ~Zhash () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zhash *zhash;
        zhash = new Zhash ();
        if (zhash) {
            zhash->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_destroy (&zhash->self);
    }

    static NAN_METHOD (defined) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zhash->self != NULL));
    }

    static NAN_METHOD (insert) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_insert (zhash->self);
    }

    static NAN_METHOD (update) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_update (zhash->self);
    }

    static NAN_METHOD (delete) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_delete (zhash->self);
    }

    static NAN_METHOD (lookup) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_lookup (zhash->self);
    }

    static NAN_METHOD (rename) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_rename (zhash->self);
    }

    static NAN_METHOD (freefn) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_freefn (zhash->self);
    }

    static NAN_METHOD (size) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_size (zhash->self);
    }

    static NAN_METHOD (dup) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_dup (zhash->self);
    }

    static NAN_METHOD (keys) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_keys (zhash->self);
    }

    static NAN_METHOD (first) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_first (zhash->self);
    }

    static NAN_METHOD (next) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_next (zhash->self);
    }

    static NAN_METHOD (cursor) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_cursor (zhash->self);
    }

    static NAN_METHOD (comment) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_comment (zhash->self);
    }

    static NAN_METHOD (pack) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_pack (zhash->self);
    }

    static NAN_METHOD (save) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_save (zhash->self);
    }

    static NAN_METHOD (load) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_load (zhash->self);
    }

    static NAN_METHOD (refresh) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_refresh (zhash->self);
    }

    static NAN_METHOD (autofree) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_autofree (zhash->self);
    }

    static NAN_METHOD (foreach) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_foreach (zhash->self);
    }

    static NAN_METHOD (test) {
        Zhash *zhash = Nan::ObjectWrap::Unwrap <Zhash> (info.Holder ());
        zhash_test (zhash->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zhash_t *self;
    public:
        zhash_t *get_self () {
            return self;
        }
};

class Zhashx: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zhashx").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "insert", insert);
            Nan::SetPrototypeMethod (tpl, "update", update);
            Nan::SetPrototypeMethod (tpl, "delete", delete);
            Nan::SetPrototypeMethod (tpl, "purge", purge);
            Nan::SetPrototypeMethod (tpl, "lookup", lookup);
            Nan::SetPrototypeMethod (tpl, "rename", rename);
            Nan::SetPrototypeMethod (tpl, "freefn", freefn);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "keys", keys);
            Nan::SetPrototypeMethod (tpl, "values", values);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "cursor", cursor);
            Nan::SetPrototypeMethod (tpl, "comment", comment);
            Nan::SetPrototypeMethod (tpl, "save", save);
            Nan::SetPrototypeMethod (tpl, "load", load);
            Nan::SetPrototypeMethod (tpl, "refresh", refresh);
            Nan::SetPrototypeMethod (tpl, "pack", pack);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "setDestructor", set_destructor);
            Nan::SetPrototypeMethod (tpl, "setDuplicator", set_duplicator);
            Nan::SetPrototypeMethod (tpl, "setKeyDestructor", set_key_destructor);
            Nan::SetPrototypeMethod (tpl, "setKeyDuplicator", set_key_duplicator);
            Nan::SetPrototypeMethod (tpl, "setKeyComparator", set_key_comparator);
            Nan::SetPrototypeMethod (tpl, "setKeyHasher", set_key_hasher);
            Nan::SetPrototypeMethod (tpl, "dupV2", dup_v2);
            Nan::SetPrototypeMethod (tpl, "autofree", autofree);
            Nan::SetPrototypeMethod (tpl, "foreach", foreach);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zhashx").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zhashx (arguments) {
            self = new (name);
        }
        ~Zhashx () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zhashx *zhashx;
        zhashx = new Zhashx ();
        if (zhashx) {
            zhashx->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_destroy (&zhashx->self);
    }

    static NAN_METHOD (defined) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zhashx->self != NULL));
    }

    static NAN_METHOD (insert) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_insert (zhashx->self);
    }

    static NAN_METHOD (update) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_update (zhashx->self);
    }

    static NAN_METHOD (delete) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_delete (zhashx->self);
    }

    static NAN_METHOD (purge) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_purge (zhashx->self);
    }

    static NAN_METHOD (lookup) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_lookup (zhashx->self);
    }

    static NAN_METHOD (rename) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_rename (zhashx->self);
    }

    static NAN_METHOD (freefn) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_freefn (zhashx->self);
    }

    static NAN_METHOD (size) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_size (zhashx->self);
    }

    static NAN_METHOD (keys) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_keys (zhashx->self);
    }

    static NAN_METHOD (values) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_values (zhashx->self);
    }

    static NAN_METHOD (first) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_first (zhashx->self);
    }

    static NAN_METHOD (next) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_next (zhashx->self);
    }

    static NAN_METHOD (cursor) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_cursor (zhashx->self);
    }

    static NAN_METHOD (comment) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_comment (zhashx->self);
    }

    static NAN_METHOD (save) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_save (zhashx->self);
    }

    static NAN_METHOD (load) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_load (zhashx->self);
    }

    static NAN_METHOD (refresh) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_refresh (zhashx->self);
    }

    static NAN_METHOD (pack) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_pack (zhashx->self);
    }

    static NAN_METHOD (dup) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_dup (zhashx->self);
    }

    static NAN_METHOD (set_destructor) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_destructor (zhashx->self);
    }

    static NAN_METHOD (set_duplicator) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_duplicator (zhashx->self);
    }

    static NAN_METHOD (set_key_destructor) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_key_destructor (zhashx->self);
    }

    static NAN_METHOD (set_key_duplicator) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_key_duplicator (zhashx->self);
    }

    static NAN_METHOD (set_key_comparator) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_key_comparator (zhashx->self);
    }

    static NAN_METHOD (set_key_hasher) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_set_key_hasher (zhashx->self);
    }

    static NAN_METHOD (dup_v2) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_dup_v2 (zhashx->self);
    }

    static NAN_METHOD (autofree) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_autofree (zhashx->self);
    }

    static NAN_METHOD (foreach) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_foreach (zhashx->self);
    }

    static NAN_METHOD (test) {
        Zhashx *zhashx = Nan::ObjectWrap::Unwrap <Zhashx> (info.Holder ());
        zhashx_test (zhashx->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zhashx_t *self;
    public:
        zhashx_t *get_self () {
            return self;
        }
};

class Ziflist: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Ziflist").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "reload", reload);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "address", address);
            Nan::SetPrototypeMethod (tpl, "broadcast", broadcast);
            Nan::SetPrototypeMethod (tpl, "netmask", netmask);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Ziflist").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Ziflist (arguments) {
            self = new (name);
        }
        ~Ziflist () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Ziflist *ziflist;
        ziflist = new Ziflist ();
        if (ziflist) {
            ziflist->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_destroy (&ziflist->self);
    }

    static NAN_METHOD (defined) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (ziflist->self != NULL));
    }

    static NAN_METHOD (reload) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_reload (ziflist->self);
    }

    static NAN_METHOD (size) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_size (ziflist->self);
    }

    static NAN_METHOD (first) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_first (ziflist->self);
    }

    static NAN_METHOD (next) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_next (ziflist->self);
    }

    static NAN_METHOD (address) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_address (ziflist->self);
    }

    static NAN_METHOD (broadcast) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_broadcast (ziflist->self);
    }

    static NAN_METHOD (netmask) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_netmask (ziflist->self);
    }

    static NAN_METHOD (print) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_print (ziflist->self);
    }

    static NAN_METHOD (test) {
        Ziflist *ziflist = Nan::ObjectWrap::Unwrap <Ziflist> (info.Holder ());
        ziflist_test (ziflist->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    ziflist_t *self;
    public:
        ziflist_t *get_self () {
            return self;
        }
};

class Zlist: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zlist").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "last", last);
            Nan::SetPrototypeMethod (tpl, "head", head);
            Nan::SetPrototypeMethod (tpl, "tail", tail);
            Nan::SetPrototypeMethod (tpl, "item", item);
            Nan::SetPrototypeMethod (tpl, "append", append);
            Nan::SetPrototypeMethod (tpl, "push", push);
            Nan::SetPrototypeMethod (tpl, "pop", pop);
            Nan::SetPrototypeMethod (tpl, "exists", exists);
            Nan::SetPrototypeMethod (tpl, "remove", remove);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "purge", purge);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "sort", sort);
            Nan::SetPrototypeMethod (tpl, "autofree", autofree);
            Nan::SetPrototypeMethod (tpl, "comparefn", comparefn);
            Nan::SetPrototypeMethod (tpl, "freefn", freefn);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zlist").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zlist (arguments) {
            self = new (name);
        }
        ~Zlist () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zlist *zlist;
        zlist = new Zlist ();
        if (zlist) {
            zlist->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_destroy (&zlist->self);
    }

    static NAN_METHOD (defined) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zlist->self != NULL));
    }

    static NAN_METHOD (first) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_first (zlist->self);
    }

    static NAN_METHOD (next) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_next (zlist->self);
    }

    static NAN_METHOD (last) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_last (zlist->self);
    }

    static NAN_METHOD (head) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_head (zlist->self);
    }

    static NAN_METHOD (tail) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_tail (zlist->self);
    }

    static NAN_METHOD (item) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_item (zlist->self);
    }

    static NAN_METHOD (append) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_append (zlist->self);
    }

    static NAN_METHOD (push) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_push (zlist->self);
    }

    static NAN_METHOD (pop) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_pop (zlist->self);
    }

    static NAN_METHOD (exists) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_exists (zlist->self);
    }

    static NAN_METHOD (remove) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_remove (zlist->self);
    }

    static NAN_METHOD (dup) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_dup (zlist->self);
    }

    static NAN_METHOD (purge) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_purge (zlist->self);
    }

    static NAN_METHOD (size) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_size (zlist->self);
    }

    static NAN_METHOD (sort) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_sort (zlist->self);
    }

    static NAN_METHOD (autofree) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_autofree (zlist->self);
    }

    static NAN_METHOD (comparefn) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_comparefn (zlist->self);
    }

    static NAN_METHOD (freefn) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_freefn (zlist->self);
    }

    static NAN_METHOD (test) {
        Zlist *zlist = Nan::ObjectWrap::Unwrap <Zlist> (info.Holder ());
        zlist_test (zlist->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zlist_t *self;
    public:
        zlist_t *get_self () {
            return self;
        }
};

class Zlistx: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zlistx").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "addStart", add_start);
            Nan::SetPrototypeMethod (tpl, "addEnd", add_end);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "head", head);
            Nan::SetPrototypeMethod (tpl, "tail", tail);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "prev", prev);
            Nan::SetPrototypeMethod (tpl, "last", last);
            Nan::SetPrototypeMethod (tpl, "item", item);
            Nan::SetPrototypeMethod (tpl, "cursor", cursor);
            Nan::SetPrototypeMethod (tpl, "handleItem", handle_item);
            Nan::SetPrototypeMethod (tpl, "find", find);
            Nan::SetPrototypeMethod (tpl, "detach", detach);
            Nan::SetPrototypeMethod (tpl, "detachCur", detach_cur);
            Nan::SetPrototypeMethod (tpl, "delete", delete);
            Nan::SetPrototypeMethod (tpl, "moveStart", move_start);
            Nan::SetPrototypeMethod (tpl, "moveEnd", move_end);
            Nan::SetPrototypeMethod (tpl, "purge", purge);
            Nan::SetPrototypeMethod (tpl, "sort", sort);
            Nan::SetPrototypeMethod (tpl, "insert", insert);
            Nan::SetPrototypeMethod (tpl, "reorder", reorder);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "setDestructor", set_destructor);
            Nan::SetPrototypeMethod (tpl, "setDuplicator", set_duplicator);
            Nan::SetPrototypeMethod (tpl, "setComparator", set_comparator);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zlistx").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zlistx (arguments) {
            self = new (name);
        }
        ~Zlistx () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zlistx *zlistx;
        zlistx = new Zlistx ();
        if (zlistx) {
            zlistx->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_destroy (&zlistx->self);
    }

    static NAN_METHOD (defined) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zlistx->self != NULL));
    }

    static NAN_METHOD (add_start) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_add_start (zlistx->self);
    }

    static NAN_METHOD (add_end) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_add_end (zlistx->self);
    }

    static NAN_METHOD (size) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_size (zlistx->self);
    }

    static NAN_METHOD (head) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_head (zlistx->self);
    }

    static NAN_METHOD (tail) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_tail (zlistx->self);
    }

    static NAN_METHOD (first) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_first (zlistx->self);
    }

    static NAN_METHOD (next) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_next (zlistx->self);
    }

    static NAN_METHOD (prev) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_prev (zlistx->self);
    }

    static NAN_METHOD (last) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_last (zlistx->self);
    }

    static NAN_METHOD (item) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_item (zlistx->self);
    }

    static NAN_METHOD (cursor) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_cursor (zlistx->self);
    }

    static NAN_METHOD (handle_item) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_handle_item (zlistx->self);
    }

    static NAN_METHOD (find) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_find (zlistx->self);
    }

    static NAN_METHOD (detach) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_detach (zlistx->self);
    }

    static NAN_METHOD (detach_cur) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_detach_cur (zlistx->self);
    }

    static NAN_METHOD (delete) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_delete (zlistx->self);
    }

    static NAN_METHOD (move_start) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_move_start (zlistx->self);
    }

    static NAN_METHOD (move_end) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_move_end (zlistx->self);
    }

    static NAN_METHOD (purge) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_purge (zlistx->self);
    }

    static NAN_METHOD (sort) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_sort (zlistx->self);
    }

    static NAN_METHOD (insert) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_insert (zlistx->self);
    }

    static NAN_METHOD (reorder) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_reorder (zlistx->self);
    }

    static NAN_METHOD (dup) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_dup (zlistx->self);
    }

    static NAN_METHOD (set_destructor) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_set_destructor (zlistx->self);
    }

    static NAN_METHOD (set_duplicator) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_set_duplicator (zlistx->self);
    }

    static NAN_METHOD (set_comparator) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_set_comparator (zlistx->self);
    }

    static NAN_METHOD (test) {
        Zlistx *zlistx = Nan::ObjectWrap::Unwrap <Zlistx> (info.Holder ());
        zlistx_test (zlistx->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zlistx_t *self;
    public:
        zlistx_t *get_self () {
            return self;
        }
};

class Zloop: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zloop").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "reader", reader);
            Nan::SetPrototypeMethod (tpl, "readerEnd", reader_end);
            Nan::SetPrototypeMethod (tpl, "readerSetTolerant", reader_set_tolerant);
            Nan::SetPrototypeMethod (tpl, "poller", poller);
            Nan::SetPrototypeMethod (tpl, "pollerEnd", poller_end);
            Nan::SetPrototypeMethod (tpl, "pollerSetTolerant", poller_set_tolerant);
            Nan::SetPrototypeMethod (tpl, "timer", timer);
            Nan::SetPrototypeMethod (tpl, "timerEnd", timer_end);
            Nan::SetPrototypeMethod (tpl, "ticket", ticket);
            Nan::SetPrototypeMethod (tpl, "ticketReset", ticket_reset);
            Nan::SetPrototypeMethod (tpl, "ticketDelete", ticket_delete);
            Nan::SetPrototypeMethod (tpl, "setTicketDelay", set_ticket_delay);
            Nan::SetPrototypeMethod (tpl, "setMaxTimers", set_max_timers);
            Nan::SetPrototypeMethod (tpl, "setVerbose", set_verbose);
            Nan::SetPrototypeMethod (tpl, "setNonstop", set_nonstop);
            Nan::SetPrototypeMethod (tpl, "start", start);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zloop").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zloop (arguments) {
            self = new (name);
        }
        ~Zloop () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zloop *zloop;
        zloop = new Zloop ();
        if (zloop) {
            zloop->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_destroy (&zloop->self);
    }

    static NAN_METHOD (defined) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zloop->self != NULL));
    }

    static NAN_METHOD (reader) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_reader (zloop->self);
    }

    static NAN_METHOD (reader_end) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_reader_end (zloop->self);
    }

    static NAN_METHOD (reader_set_tolerant) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_reader_set_tolerant (zloop->self);
    }

    static NAN_METHOD (poller) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_poller (zloop->self);
    }

    static NAN_METHOD (poller_end) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_poller_end (zloop->self);
    }

    static NAN_METHOD (poller_set_tolerant) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_poller_set_tolerant (zloop->self);
    }

    static NAN_METHOD (timer) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_timer (zloop->self);
    }

    static NAN_METHOD (timer_end) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_timer_end (zloop->self);
    }

    static NAN_METHOD (ticket) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_ticket (zloop->self);
    }

    static NAN_METHOD (ticket_reset) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_ticket_reset (zloop->self);
    }

    static NAN_METHOD (ticket_delete) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_ticket_delete (zloop->self);
    }

    static NAN_METHOD (set_ticket_delay) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_set_ticket_delay (zloop->self);
    }

    static NAN_METHOD (set_max_timers) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_set_max_timers (zloop->self);
    }

    static NAN_METHOD (set_verbose) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_set_verbose (zloop->self);
    }

    static NAN_METHOD (set_nonstop) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_set_nonstop (zloop->self);
    }

    static NAN_METHOD (start) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_start (zloop->self);
    }

    static NAN_METHOD (test) {
        Zloop *zloop = Nan::ObjectWrap::Unwrap <Zloop> (info.Holder ());
        zloop_test (zloop->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zloop_t *self;
    public:
        zloop_t *get_self () {
            return self;
        }
};

class Zmsg: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zmsg").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "sendm", sendm);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "contentSize", content_size);
            Nan::SetPrototypeMethod (tpl, "routingId", routing_id);
            Nan::SetPrototypeMethod (tpl, "setRoutingId", set_routing_id);
            Nan::SetPrototypeMethod (tpl, "prepend", prepend);
            Nan::SetPrototypeMethod (tpl, "append", append);
            Nan::SetPrototypeMethod (tpl, "pop", pop);
            Nan::SetPrototypeMethod (tpl, "pushmem", pushmem);
            Nan::SetPrototypeMethod (tpl, "addmem", addmem);
            Nan::SetPrototypeMethod (tpl, "pushstr", pushstr);
            Nan::SetPrototypeMethod (tpl, "addstr", addstr);
            Nan::SetPrototypeMethod (tpl, "pushstrf", pushstrf);
            Nan::SetPrototypeMethod (tpl, "addstrf", addstrf);
            Nan::SetPrototypeMethod (tpl, "popstr", popstr);
            Nan::SetPrototypeMethod (tpl, "addmsg", addmsg);
            Nan::SetPrototypeMethod (tpl, "popmsg", popmsg);
            Nan::SetPrototypeMethod (tpl, "remove", remove);
            Nan::SetPrototypeMethod (tpl, "first", first);
            Nan::SetPrototypeMethod (tpl, "next", next);
            Nan::SetPrototypeMethod (tpl, "last", last);
            Nan::SetPrototypeMethod (tpl, "save", save);
            Nan::SetPrototypeMethod (tpl, "encode", encode);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "eq", eq);
            Nan::SetPrototypeMethod (tpl, "signal", signal);
            Nan::SetPrototypeMethod (tpl, "is", is);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zmsg").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zmsg (arguments) {
            self = new (name);
        }
        ~Zmsg () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zmsg *zmsg;
        zmsg = new Zmsg ();
        if (zmsg) {
            zmsg->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_destroy (&zmsg->self);
    }

    static NAN_METHOD (defined) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zmsg->self != NULL));
    }

    static NAN_METHOD (send) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_send (zmsg->self);
    }

    static NAN_METHOD (sendm) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_sendm (zmsg->self);
    }

    static NAN_METHOD (size) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_size (zmsg->self);
    }

    static NAN_METHOD (content_size) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_content_size (zmsg->self);
    }

    static NAN_METHOD (routing_id) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_routing_id (zmsg->self);
    }

    static NAN_METHOD (set_routing_id) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_set_routing_id (zmsg->self);
    }

    static NAN_METHOD (prepend) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_prepend (zmsg->self);
    }

    static NAN_METHOD (append) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_append (zmsg->self);
    }

    static NAN_METHOD (pop) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_pop (zmsg->self);
    }

    static NAN_METHOD (pushmem) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_pushmem (zmsg->self);
    }

    static NAN_METHOD (addmem) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_addmem (zmsg->self);
    }

    static NAN_METHOD (pushstr) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_pushstr (zmsg->self);
    }

    static NAN_METHOD (addstr) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_addstr (zmsg->self);
    }

    static NAN_METHOD (pushstrf) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_pushstrf (zmsg->self);
    }

    static NAN_METHOD (addstrf) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_addstrf (zmsg->self);
    }

    static NAN_METHOD (popstr) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_popstr (zmsg->self);
    }

    static NAN_METHOD (addmsg) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_addmsg (zmsg->self);
    }

    static NAN_METHOD (popmsg) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_popmsg (zmsg->self);
    }

    static NAN_METHOD (remove) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_remove (zmsg->self);
    }

    static NAN_METHOD (first) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_first (zmsg->self);
    }

    static NAN_METHOD (next) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_next (zmsg->self);
    }

    static NAN_METHOD (last) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_last (zmsg->self);
    }

    static NAN_METHOD (save) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_save (zmsg->self);
    }

    static NAN_METHOD (encode) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_encode (zmsg->self);
    }

    static NAN_METHOD (dup) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_dup (zmsg->self);
    }

    static NAN_METHOD (print) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_print (zmsg->self);
    }

    static NAN_METHOD (eq) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_eq (zmsg->self);
    }

    static NAN_METHOD (signal) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_signal (zmsg->self);
    }

    static NAN_METHOD (is) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_is (zmsg->self);
    }

    static NAN_METHOD (test) {
        Zmsg *zmsg = Nan::ObjectWrap::Unwrap <Zmsg> (info.Holder ());
        zmsg_test (zmsg->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zmsg_t *self;
    public:
        zmsg_t *get_self () {
            return self;
        }
};

class Zpoller: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zpoller").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "add", add);
            Nan::SetPrototypeMethod (tpl, "remove", remove);
            Nan::SetPrototypeMethod (tpl, "setNonstop", set_nonstop);
            Nan::SetPrototypeMethod (tpl, "wait", wait);
            Nan::SetPrototypeMethod (tpl, "expired", expired);
            Nan::SetPrototypeMethod (tpl, "terminated", terminated);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zpoller").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zpoller (arguments) {
            self = new (name);
        }
        ~Zpoller () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zpoller *zpoller;
        zpoller = new Zpoller ();
        if (zpoller) {
            zpoller->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_destroy (&zpoller->self);
    }

    static NAN_METHOD (defined) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zpoller->self != NULL));
    }

    static NAN_METHOD (add) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_add (zpoller->self);
    }

    static NAN_METHOD (remove) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_remove (zpoller->self);
    }

    static NAN_METHOD (set_nonstop) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_set_nonstop (zpoller->self);
    }

    static NAN_METHOD (wait) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_wait (zpoller->self);
    }

    static NAN_METHOD (expired) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_expired (zpoller->self);
    }

    static NAN_METHOD (terminated) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_terminated (zpoller->self);
    }

    static NAN_METHOD (test) {
        Zpoller *zpoller = Nan::ObjectWrap::Unwrap <Zpoller> (info.Holder ());
        zpoller_test (zpoller->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zpoller_t *self;
    public:
        zpoller_t *get_self () {
            return self;
        }
};

class Zproc: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zproc").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "czmqVersion", czmq_version);
            Nan::SetPrototypeMethod (tpl, "interrupted", interrupted);
            Nan::SetPrototypeMethod (tpl, "hasCurve", has_curve);
            Nan::SetPrototypeMethod (tpl, "hostname", hostname);
            Nan::SetPrototypeMethod (tpl, "daemonize", daemonize);
            Nan::SetPrototypeMethod (tpl, "runAs", run_as);
            Nan::SetPrototypeMethod (tpl, "setIoThreads", set_io_threads);
            Nan::SetPrototypeMethod (tpl, "setMaxSockets", set_max_sockets);
            Nan::SetPrototypeMethod (tpl, "setBiface", set_biface);
            Nan::SetPrototypeMethod (tpl, "biface", biface);
            Nan::SetPrototypeMethod (tpl, "setLogIdent", set_log_ident);
            Nan::SetPrototypeMethod (tpl, "setLogSender", set_log_sender);
            Nan::SetPrototypeMethod (tpl, "setLogSystem", set_log_system);
            Nan::SetPrototypeMethod (tpl, "logError", log_error);
            Nan::SetPrototypeMethod (tpl, "logWarning", log_warning);
            Nan::SetPrototypeMethod (tpl, "logNotice", log_notice);
            Nan::SetPrototypeMethod (tpl, "logInfo", log_info);
            Nan::SetPrototypeMethod (tpl, "logDebug", log_debug);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zproc").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zproc () {
        }
        ~Zproc () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zproc *zproc;
        zproc = new Zproc ();
        if (zproc) {
            zproc->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (czmq_version) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_czmq_version (zproc->self);
    }

    static NAN_METHOD (interrupted) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_interrupted (zproc->self);
    }

    static NAN_METHOD (has_curve) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_has_curve (zproc->self);
    }

    static NAN_METHOD (hostname) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_hostname (zproc->self);
    }

    static NAN_METHOD (daemonize) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_daemonize (zproc->self);
    }

    static NAN_METHOD (run_as) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_run_as (zproc->self);
    }

    static NAN_METHOD (set_io_threads) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_io_threads (zproc->self);
    }

    static NAN_METHOD (set_max_sockets) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_max_sockets (zproc->self);
    }

    static NAN_METHOD (set_biface) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_biface (zproc->self);
    }

    static NAN_METHOD (biface) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_biface (zproc->self);
    }

    static NAN_METHOD (set_log_ident) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_log_ident (zproc->self);
    }

    static NAN_METHOD (set_log_sender) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_log_sender (zproc->self);
    }

    static NAN_METHOD (set_log_system) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_set_log_system (zproc->self);
    }

    static NAN_METHOD (log_error) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_log_error (zproc->self);
    }

    static NAN_METHOD (log_warning) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_log_warning (zproc->self);
    }

    static NAN_METHOD (log_notice) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_log_notice (zproc->self);
    }

    static NAN_METHOD (log_info) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_log_info (zproc->self);
    }

    static NAN_METHOD (log_debug) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_log_debug (zproc->self);
    }

    static NAN_METHOD (test) {
        Zproc *zproc = Nan::ObjectWrap::Unwrap <Zproc> (info.Holder ());
        zproc_test (zproc->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zsock: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zsock").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "bind", bind);
            Nan::SetPrototypeMethod (tpl, "endpoint", endpoint);
            Nan::SetPrototypeMethod (tpl, "unbind", unbind);
            Nan::SetPrototypeMethod (tpl, "connect", connect);
            Nan::SetPrototypeMethod (tpl, "disconnect", disconnect);
            Nan::SetPrototypeMethod (tpl, "attach", attach);
            Nan::SetPrototypeMethod (tpl, "typeStr", type_str);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "vsend", vsend);
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "vrecv", vrecv);
            Nan::SetPrototypeMethod (tpl, "bsend", bsend);
            Nan::SetPrototypeMethod (tpl, "brecv", brecv);
            Nan::SetPrototypeMethod (tpl, "routingId", routing_id);
            Nan::SetPrototypeMethod (tpl, "setRoutingId", set_routing_id);
            Nan::SetPrototypeMethod (tpl, "setUnbounded", set_unbounded);
            Nan::SetPrototypeMethod (tpl, "signal", signal);
            Nan::SetPrototypeMethod (tpl, "wait", wait);
            Nan::SetPrototypeMethod (tpl, "flush", flush);
            Nan::SetPrototypeMethod (tpl, "join", join);
            Nan::SetPrototypeMethod (tpl, "leave", leave);
            Nan::SetPrototypeMethod (tpl, "is", is);
            Nan::SetPrototypeMethod (tpl, "resolve", resolve);
            Nan::SetPrototypeMethod (tpl, "heartbeatIvl", heartbeat_ivl);
            Nan::SetPrototypeMethod (tpl, "setHeartbeatIvl", set_heartbeat_ivl);
            Nan::SetPrototypeMethod (tpl, "heartbeatTtl", heartbeat_ttl);
            Nan::SetPrototypeMethod (tpl, "setHeartbeatTtl", set_heartbeat_ttl);
            Nan::SetPrototypeMethod (tpl, "heartbeatTimeout", heartbeat_timeout);
            Nan::SetPrototypeMethod (tpl, "setHeartbeatTimeout", set_heartbeat_timeout);
            Nan::SetPrototypeMethod (tpl, "useFd", use_fd);
            Nan::SetPrototypeMethod (tpl, "setUseFd", set_use_fd);
            Nan::SetPrototypeMethod (tpl, "tos", tos);
            Nan::SetPrototypeMethod (tpl, "setTos", set_tos);
            Nan::SetPrototypeMethod (tpl, "setRouterHandover", set_router_handover);
            Nan::SetPrototypeMethod (tpl, "setRouterMandatory", set_router_mandatory);
            Nan::SetPrototypeMethod (tpl, "setProbeRouter", set_probe_router);
            Nan::SetPrototypeMethod (tpl, "setReqRelaxed", set_req_relaxed);
            Nan::SetPrototypeMethod (tpl, "setReqCorrelate", set_req_correlate);
            Nan::SetPrototypeMethod (tpl, "setConflate", set_conflate);
            Nan::SetPrototypeMethod (tpl, "zapDomain", zap_domain);
            Nan::SetPrototypeMethod (tpl, "setZapDomain", set_zap_domain);
            Nan::SetPrototypeMethod (tpl, "mechanism", mechanism);
            Nan::SetPrototypeMethod (tpl, "plainServer", plain_server);
            Nan::SetPrototypeMethod (tpl, "setPlainServer", set_plain_server);
            Nan::SetPrototypeMethod (tpl, "plainUsername", plain_username);
            Nan::SetPrototypeMethod (tpl, "setPlainUsername", set_plain_username);
            Nan::SetPrototypeMethod (tpl, "plainPassword", plain_password);
            Nan::SetPrototypeMethod (tpl, "setPlainPassword", set_plain_password);
            Nan::SetPrototypeMethod (tpl, "curveServer", curve_server);
            Nan::SetPrototypeMethod (tpl, "setCurveServer", set_curve_server);
            Nan::SetPrototypeMethod (tpl, "curvePublickey", curve_publickey);
            Nan::SetPrototypeMethod (tpl, "setCurvePublickey", set_curve_publickey);
            Nan::SetPrototypeMethod (tpl, "setCurvePublickeyBin", set_curve_publickey_bin);
            Nan::SetPrototypeMethod (tpl, "curveSecretkey", curve_secretkey);
            Nan::SetPrototypeMethod (tpl, "setCurveSecretkey", set_curve_secretkey);
            Nan::SetPrototypeMethod (tpl, "setCurveSecretkeyBin", set_curve_secretkey_bin);
            Nan::SetPrototypeMethod (tpl, "curveServerkey", curve_serverkey);
            Nan::SetPrototypeMethod (tpl, "setCurveServerkey", set_curve_serverkey);
            Nan::SetPrototypeMethod (tpl, "setCurveServerkeyBin", set_curve_serverkey_bin);
            Nan::SetPrototypeMethod (tpl, "gssapiServer", gssapi_server);
            Nan::SetPrototypeMethod (tpl, "setGssapiServer", set_gssapi_server);
            Nan::SetPrototypeMethod (tpl, "gssapiPlaintext", gssapi_plaintext);
            Nan::SetPrototypeMethod (tpl, "setGssapiPlaintext", set_gssapi_plaintext);
            Nan::SetPrototypeMethod (tpl, "gssapiPrincipal", gssapi_principal);
            Nan::SetPrototypeMethod (tpl, "setGssapiPrincipal", set_gssapi_principal);
            Nan::SetPrototypeMethod (tpl, "gssapiServicePrincipal", gssapi_service_principal);
            Nan::SetPrototypeMethod (tpl, "setGssapiServicePrincipal", set_gssapi_service_principal);
            Nan::SetPrototypeMethod (tpl, "ipv6", ipv6);
            Nan::SetPrototypeMethod (tpl, "setIpv6", set_ipv6);
            Nan::SetPrototypeMethod (tpl, "immediate", immediate);
            Nan::SetPrototypeMethod (tpl, "setImmediate", set_immediate);
            Nan::SetPrototypeMethod (tpl, "setRouterRaw", set_router_raw);
            Nan::SetPrototypeMethod (tpl, "ipv4only", ipv4only);
            Nan::SetPrototypeMethod (tpl, "setIpv4only", set_ipv4only);
            Nan::SetPrototypeMethod (tpl, "setDelayAttachOnConnect", set_delay_attach_on_connect);
            Nan::SetPrototypeMethod (tpl, "type", type);
            Nan::SetPrototypeMethod (tpl, "sndhwm", sndhwm);
            Nan::SetPrototypeMethod (tpl, "setSndhwm", set_sndhwm);
            Nan::SetPrototypeMethod (tpl, "rcvhwm", rcvhwm);
            Nan::SetPrototypeMethod (tpl, "setRcvhwm", set_rcvhwm);
            Nan::SetPrototypeMethod (tpl, "affinity", affinity);
            Nan::SetPrototypeMethod (tpl, "setAffinity", set_affinity);
            Nan::SetPrototypeMethod (tpl, "setSubscribe", set_subscribe);
            Nan::SetPrototypeMethod (tpl, "setUnsubscribe", set_unsubscribe);
            Nan::SetPrototypeMethod (tpl, "identity", identity);
            Nan::SetPrototypeMethod (tpl, "setIdentity", set_identity);
            Nan::SetPrototypeMethod (tpl, "rate", rate);
            Nan::SetPrototypeMethod (tpl, "setRate", set_rate);
            Nan::SetPrototypeMethod (tpl, "recoveryIvl", recovery_ivl);
            Nan::SetPrototypeMethod (tpl, "setRecoveryIvl", set_recovery_ivl);
            Nan::SetPrototypeMethod (tpl, "sndbuf", sndbuf);
            Nan::SetPrototypeMethod (tpl, "setSndbuf", set_sndbuf);
            Nan::SetPrototypeMethod (tpl, "rcvbuf", rcvbuf);
            Nan::SetPrototypeMethod (tpl, "setRcvbuf", set_rcvbuf);
            Nan::SetPrototypeMethod (tpl, "linger", linger);
            Nan::SetPrototypeMethod (tpl, "setLinger", set_linger);
            Nan::SetPrototypeMethod (tpl, "reconnectIvl", reconnect_ivl);
            Nan::SetPrototypeMethod (tpl, "setReconnectIvl", set_reconnect_ivl);
            Nan::SetPrototypeMethod (tpl, "reconnectIvlMax", reconnect_ivl_max);
            Nan::SetPrototypeMethod (tpl, "setReconnectIvlMax", set_reconnect_ivl_max);
            Nan::SetPrototypeMethod (tpl, "backlog", backlog);
            Nan::SetPrototypeMethod (tpl, "setBacklog", set_backlog);
            Nan::SetPrototypeMethod (tpl, "maxmsgsize", maxmsgsize);
            Nan::SetPrototypeMethod (tpl, "setMaxmsgsize", set_maxmsgsize);
            Nan::SetPrototypeMethod (tpl, "multicastHops", multicast_hops);
            Nan::SetPrototypeMethod (tpl, "setMulticastHops", set_multicast_hops);
            Nan::SetPrototypeMethod (tpl, "rcvtimeo", rcvtimeo);
            Nan::SetPrototypeMethod (tpl, "setRcvtimeo", set_rcvtimeo);
            Nan::SetPrototypeMethod (tpl, "sndtimeo", sndtimeo);
            Nan::SetPrototypeMethod (tpl, "setSndtimeo", set_sndtimeo);
            Nan::SetPrototypeMethod (tpl, "setXpubVerbose", set_xpub_verbose);
            Nan::SetPrototypeMethod (tpl, "tcpKeepalive", tcp_keepalive);
            Nan::SetPrototypeMethod (tpl, "setTcpKeepalive", set_tcp_keepalive);
            Nan::SetPrototypeMethod (tpl, "tcpKeepaliveIdle", tcp_keepalive_idle);
            Nan::SetPrototypeMethod (tpl, "setTcpKeepaliveIdle", set_tcp_keepalive_idle);
            Nan::SetPrototypeMethod (tpl, "tcpKeepaliveCnt", tcp_keepalive_cnt);
            Nan::SetPrototypeMethod (tpl, "setTcpKeepaliveCnt", set_tcp_keepalive_cnt);
            Nan::SetPrototypeMethod (tpl, "tcpKeepaliveIntvl", tcp_keepalive_intvl);
            Nan::SetPrototypeMethod (tpl, "setTcpKeepaliveIntvl", set_tcp_keepalive_intvl);
            Nan::SetPrototypeMethod (tpl, "tcpAcceptFilter", tcp_accept_filter);
            Nan::SetPrototypeMethod (tpl, "setTcpAcceptFilter", set_tcp_accept_filter);
            Nan::SetPrototypeMethod (tpl, "rcvmore", rcvmore);
            Nan::SetPrototypeMethod (tpl, "fd", fd);
            Nan::SetPrototypeMethod (tpl, "events", events);
            Nan::SetPrototypeMethod (tpl, "lastEndpoint", last_endpoint);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zsock").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zsock (arguments) {
            self = new (name);
        }
        ~Zsock () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zsock *zsock;
        zsock = new Zsock ();
        if (zsock) {
            zsock->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_destroy (&zsock->self);
    }

    static NAN_METHOD (defined) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zsock->self != NULL));
    }

    static NAN_METHOD (bind) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_bind (zsock->self);
    }

    static NAN_METHOD (endpoint) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_endpoint (zsock->self);
    }

    static NAN_METHOD (unbind) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_unbind (zsock->self);
    }

    static NAN_METHOD (connect) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_connect (zsock->self);
    }

    static NAN_METHOD (disconnect) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_disconnect (zsock->self);
    }

    static NAN_METHOD (attach) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_attach (zsock->self);
    }

    static NAN_METHOD (type_str) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_type_str (zsock->self);
    }

    static NAN_METHOD (send) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_send (zsock->self);
    }

    static NAN_METHOD (vsend) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_vsend (zsock->self);
    }

    static NAN_METHOD (recv) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_recv (zsock->self);
    }

    static NAN_METHOD (vrecv) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_vrecv (zsock->self);
    }

    static NAN_METHOD (bsend) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_bsend (zsock->self);
    }

    static NAN_METHOD (brecv) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_brecv (zsock->self);
    }

    static NAN_METHOD (routing_id) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_routing_id (zsock->self);
    }

    static NAN_METHOD (set_routing_id) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_routing_id (zsock->self);
    }

    static NAN_METHOD (set_unbounded) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_unbounded (zsock->self);
    }

    static NAN_METHOD (signal) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_signal (zsock->self);
    }

    static NAN_METHOD (wait) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_wait (zsock->self);
    }

    static NAN_METHOD (flush) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_flush (zsock->self);
    }

    static NAN_METHOD (join) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_join (zsock->self);
    }

    static NAN_METHOD (leave) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_leave (zsock->self);
    }

    static NAN_METHOD (is) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_is (zsock->self);
    }

    static NAN_METHOD (resolve) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_resolve (zsock->self);
    }

    static NAN_METHOD (heartbeat_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_heartbeat_ivl (zsock->self);
    }

    static NAN_METHOD (set_heartbeat_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_heartbeat_ivl (zsock->self);
    }

    static NAN_METHOD (heartbeat_ttl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_heartbeat_ttl (zsock->self);
    }

    static NAN_METHOD (set_heartbeat_ttl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_heartbeat_ttl (zsock->self);
    }

    static NAN_METHOD (heartbeat_timeout) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_heartbeat_timeout (zsock->self);
    }

    static NAN_METHOD (set_heartbeat_timeout) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_heartbeat_timeout (zsock->self);
    }

    static NAN_METHOD (use_fd) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_use_fd (zsock->self);
    }

    static NAN_METHOD (set_use_fd) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_use_fd (zsock->self);
    }

    static NAN_METHOD (tos) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tos (zsock->self);
    }

    static NAN_METHOD (set_tos) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tos (zsock->self);
    }

    static NAN_METHOD (set_router_handover) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_router_handover (zsock->self);
    }

    static NAN_METHOD (set_router_mandatory) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_router_mandatory (zsock->self);
    }

    static NAN_METHOD (set_probe_router) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_probe_router (zsock->self);
    }

    static NAN_METHOD (set_req_relaxed) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_req_relaxed (zsock->self);
    }

    static NAN_METHOD (set_req_correlate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_req_correlate (zsock->self);
    }

    static NAN_METHOD (set_conflate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_conflate (zsock->self);
    }

    static NAN_METHOD (zap_domain) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_zap_domain (zsock->self);
    }

    static NAN_METHOD (set_zap_domain) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_zap_domain (zsock->self);
    }

    static NAN_METHOD (mechanism) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_mechanism (zsock->self);
    }

    static NAN_METHOD (plain_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_plain_server (zsock->self);
    }

    static NAN_METHOD (set_plain_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_plain_server (zsock->self);
    }

    static NAN_METHOD (plain_username) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_plain_username (zsock->self);
    }

    static NAN_METHOD (set_plain_username) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_plain_username (zsock->self);
    }

    static NAN_METHOD (plain_password) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_plain_password (zsock->self);
    }

    static NAN_METHOD (set_plain_password) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_plain_password (zsock->self);
    }

    static NAN_METHOD (curve_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_curve_server (zsock->self);
    }

    static NAN_METHOD (set_curve_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_server (zsock->self);
    }

    static NAN_METHOD (curve_publickey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_curve_publickey (zsock->self);
    }

    static NAN_METHOD (set_curve_publickey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_publickey (zsock->self);
    }

    static NAN_METHOD (set_curve_publickey_bin) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_publickey_bin (zsock->self);
    }

    static NAN_METHOD (curve_secretkey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_curve_secretkey (zsock->self);
    }

    static NAN_METHOD (set_curve_secretkey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_secretkey (zsock->self);
    }

    static NAN_METHOD (set_curve_secretkey_bin) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_secretkey_bin (zsock->self);
    }

    static NAN_METHOD (curve_serverkey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_curve_serverkey (zsock->self);
    }

    static NAN_METHOD (set_curve_serverkey) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_serverkey (zsock->self);
    }

    static NAN_METHOD (set_curve_serverkey_bin) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_curve_serverkey_bin (zsock->self);
    }

    static NAN_METHOD (gssapi_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_gssapi_server (zsock->self);
    }

    static NAN_METHOD (set_gssapi_server) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_gssapi_server (zsock->self);
    }

    static NAN_METHOD (gssapi_plaintext) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_gssapi_plaintext (zsock->self);
    }

    static NAN_METHOD (set_gssapi_plaintext) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_gssapi_plaintext (zsock->self);
    }

    static NAN_METHOD (gssapi_principal) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_gssapi_principal (zsock->self);
    }

    static NAN_METHOD (set_gssapi_principal) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_gssapi_principal (zsock->self);
    }

    static NAN_METHOD (gssapi_service_principal) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_gssapi_service_principal (zsock->self);
    }

    static NAN_METHOD (set_gssapi_service_principal) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_gssapi_service_principal (zsock->self);
    }

    static NAN_METHOD (ipv6) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_ipv6 (zsock->self);
    }

    static NAN_METHOD (set_ipv6) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_ipv6 (zsock->self);
    }

    static NAN_METHOD (immediate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_immediate (zsock->self);
    }

    static NAN_METHOD (set_immediate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_immediate (zsock->self);
    }

    static NAN_METHOD (set_router_raw) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_router_raw (zsock->self);
    }

    static NAN_METHOD (ipv4only) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_ipv4only (zsock->self);
    }

    static NAN_METHOD (set_ipv4only) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_ipv4only (zsock->self);
    }

    static NAN_METHOD (set_delay_attach_on_connect) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_delay_attach_on_connect (zsock->self);
    }

    static NAN_METHOD (type) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_type (zsock->self);
    }

    static NAN_METHOD (sndhwm) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_sndhwm (zsock->self);
    }

    static NAN_METHOD (set_sndhwm) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_sndhwm (zsock->self);
    }

    static NAN_METHOD (rcvhwm) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_rcvhwm (zsock->self);
    }

    static NAN_METHOD (set_rcvhwm) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_rcvhwm (zsock->self);
    }

    static NAN_METHOD (affinity) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_affinity (zsock->self);
    }

    static NAN_METHOD (set_affinity) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_affinity (zsock->self);
    }

    static NAN_METHOD (set_subscribe) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_subscribe (zsock->self);
    }

    static NAN_METHOD (set_unsubscribe) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_unsubscribe (zsock->self);
    }

    static NAN_METHOD (identity) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_identity (zsock->self);
    }

    static NAN_METHOD (set_identity) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_identity (zsock->self);
    }

    static NAN_METHOD (rate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_rate (zsock->self);
    }

    static NAN_METHOD (set_rate) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_rate (zsock->self);
    }

    static NAN_METHOD (recovery_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_recovery_ivl (zsock->self);
    }

    static NAN_METHOD (set_recovery_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_recovery_ivl (zsock->self);
    }

    static NAN_METHOD (sndbuf) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_sndbuf (zsock->self);
    }

    static NAN_METHOD (set_sndbuf) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_sndbuf (zsock->self);
    }

    static NAN_METHOD (rcvbuf) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_rcvbuf (zsock->self);
    }

    static NAN_METHOD (set_rcvbuf) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_rcvbuf (zsock->self);
    }

    static NAN_METHOD (linger) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_linger (zsock->self);
    }

    static NAN_METHOD (set_linger) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_linger (zsock->self);
    }

    static NAN_METHOD (reconnect_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_reconnect_ivl (zsock->self);
    }

    static NAN_METHOD (set_reconnect_ivl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_reconnect_ivl (zsock->self);
    }

    static NAN_METHOD (reconnect_ivl_max) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_reconnect_ivl_max (zsock->self);
    }

    static NAN_METHOD (set_reconnect_ivl_max) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_reconnect_ivl_max (zsock->self);
    }

    static NAN_METHOD (backlog) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_backlog (zsock->self);
    }

    static NAN_METHOD (set_backlog) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_backlog (zsock->self);
    }

    static NAN_METHOD (maxmsgsize) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_maxmsgsize (zsock->self);
    }

    static NAN_METHOD (set_maxmsgsize) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_maxmsgsize (zsock->self);
    }

    static NAN_METHOD (multicast_hops) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_multicast_hops (zsock->self);
    }

    static NAN_METHOD (set_multicast_hops) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_multicast_hops (zsock->self);
    }

    static NAN_METHOD (rcvtimeo) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_rcvtimeo (zsock->self);
    }

    static NAN_METHOD (set_rcvtimeo) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_rcvtimeo (zsock->self);
    }

    static NAN_METHOD (sndtimeo) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_sndtimeo (zsock->self);
    }

    static NAN_METHOD (set_sndtimeo) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_sndtimeo (zsock->self);
    }

    static NAN_METHOD (set_xpub_verbose) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_xpub_verbose (zsock->self);
    }

    static NAN_METHOD (tcp_keepalive) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tcp_keepalive (zsock->self);
    }

    static NAN_METHOD (set_tcp_keepalive) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tcp_keepalive (zsock->self);
    }

    static NAN_METHOD (tcp_keepalive_idle) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tcp_keepalive_idle (zsock->self);
    }

    static NAN_METHOD (set_tcp_keepalive_idle) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tcp_keepalive_idle (zsock->self);
    }

    static NAN_METHOD (tcp_keepalive_cnt) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tcp_keepalive_cnt (zsock->self);
    }

    static NAN_METHOD (set_tcp_keepalive_cnt) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tcp_keepalive_cnt (zsock->self);
    }

    static NAN_METHOD (tcp_keepalive_intvl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tcp_keepalive_intvl (zsock->self);
    }

    static NAN_METHOD (set_tcp_keepalive_intvl) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tcp_keepalive_intvl (zsock->self);
    }

    static NAN_METHOD (tcp_accept_filter) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_tcp_accept_filter (zsock->self);
    }

    static NAN_METHOD (set_tcp_accept_filter) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_set_tcp_accept_filter (zsock->self);
    }

    static NAN_METHOD (rcvmore) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_rcvmore (zsock->self);
    }

    static NAN_METHOD (fd) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_fd (zsock->self);
    }

    static NAN_METHOD (events) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_events (zsock->self);
    }

    static NAN_METHOD (last_endpoint) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_last_endpoint (zsock->self);
    }

    static NAN_METHOD (test) {
        Zsock *zsock = Nan::ObjectWrap::Unwrap <Zsock> (info.Holder ());
        zsock_test (zsock->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zsock_t *self;
    public:
        zsock_t *get_self () {
            return self;
        }
};

class Zstr: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zstr").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "recv", recv);
            Nan::SetPrototypeMethod (tpl, "recvx", recvx);
            Nan::SetPrototypeMethod (tpl, "send", send);
            Nan::SetPrototypeMethod (tpl, "sendm", sendm);
            Nan::SetPrototypeMethod (tpl, "sendf", sendf);
            Nan::SetPrototypeMethod (tpl, "sendfm", sendfm);
            Nan::SetPrototypeMethod (tpl, "sendx", sendx);
            Nan::SetPrototypeMethod (tpl, "str", str);
            Nan::SetPrototypeMethod (tpl, "free", free);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zstr").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zstr () {
        }
        ~Zstr () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zstr *zstr;
        zstr = new Zstr ();
        if (zstr) {
            zstr->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (recv) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_recv (zstr->self);
    }

    static NAN_METHOD (recvx) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_recvx (zstr->self);
    }

    static NAN_METHOD (send) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_send (zstr->self);
    }

    static NAN_METHOD (sendm) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_sendm (zstr->self);
    }

    static NAN_METHOD (sendf) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_sendf (zstr->self);
    }

    static NAN_METHOD (sendfm) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_sendfm (zstr->self);
    }

    static NAN_METHOD (sendx) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_sendx (zstr->self);
    }

    static NAN_METHOD (str) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_str (zstr->self);
    }

    static NAN_METHOD (free) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_free (zstr->self);
    }

    static NAN_METHOD (test) {
        Zstr *zstr = Nan::ObjectWrap::Unwrap <Zstr> (info.Holder ());
        zstr_test (zstr->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Ztrie: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Ztrie").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "insertRoute", insert_route);
            Nan::SetPrototypeMethod (tpl, "removeRoute", remove_route);
            Nan::SetPrototypeMethod (tpl, "matches", matches);
            Nan::SetPrototypeMethod (tpl, "hitData", hit_data);
            Nan::SetPrototypeMethod (tpl, "hitParameterCount", hit_parameter_count);
            Nan::SetPrototypeMethod (tpl, "hitParameters", hit_parameters);
            Nan::SetPrototypeMethod (tpl, "hitAsteriskMatch", hit_asterisk_match);
            Nan::SetPrototypeMethod (tpl, "print", print);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Ztrie").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Ztrie (arguments) {
            self = new (name);
        }
        ~Ztrie () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Ztrie *ztrie;
        ztrie = new Ztrie ();
        if (ztrie) {
            ztrie->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_destroy (&ztrie->self);
    }

    static NAN_METHOD (defined) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (ztrie->self != NULL));
    }

    static NAN_METHOD (insert_route) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_insert_route (ztrie->self);
    }

    static NAN_METHOD (remove_route) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_remove_route (ztrie->self);
    }

    static NAN_METHOD (matches) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_matches (ztrie->self);
    }

    static NAN_METHOD (hit_data) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_hit_data (ztrie->self);
    }

    static NAN_METHOD (hit_parameter_count) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_hit_parameter_count (ztrie->self);
    }

    static NAN_METHOD (hit_parameters) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_hit_parameters (ztrie->self);
    }

    static NAN_METHOD (hit_asterisk_match) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_hit_asterisk_match (ztrie->self);
    }

    static NAN_METHOD (print) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_print (ztrie->self);
    }

    static NAN_METHOD (test) {
        Ztrie *ztrie = Nan::ObjectWrap::Unwrap <Ztrie> (info.Holder ());
        ztrie_test (ztrie->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    ztrie_t *self;
    public:
        ztrie_t *get_self () {
            return self;
        }
};

class Zuuid: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zuuid").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "destroy", destroy);
            Nan::SetPrototypeMethod (tpl, "set", set);
            Nan::SetPrototypeMethod (tpl, "setStr", set_str);
            Nan::SetPrototypeMethod (tpl, "data", data);
            Nan::SetPrototypeMethod (tpl, "size", size);
            Nan::SetPrototypeMethod (tpl, "str", str);
            Nan::SetPrototypeMethod (tpl, "strCanonical", str_canonical);
            Nan::SetPrototypeMethod (tpl, "export", export);
            Nan::SetPrototypeMethod (tpl, "eq", eq);
            Nan::SetPrototypeMethod (tpl, "neq", neq);
            Nan::SetPrototypeMethod (tpl, "dup", dup);
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zuuid").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zuuid (arguments) {
            self = new (name);
        }
        ~Zuuid () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zuuid *zuuid;
        zuuid = new Zuuid ();
        if (zuuid) {
            zuuid->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (destroy) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_destroy (&zuuid->self);
    }

    static NAN_METHOD (defined) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        info.GetReturnValue ().Set (Nan::New (zuuid->self != NULL));
    }

    static NAN_METHOD (set) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_set (zuuid->self);
    }

    static NAN_METHOD (set_str) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_set_str (zuuid->self);
    }

    static NAN_METHOD (data) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_data (zuuid->self);
    }

    static NAN_METHOD (size) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_size (zuuid->self);
    }

    static NAN_METHOD (str) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_str (zuuid->self);
    }

    static NAN_METHOD (str_canonical) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_str_canonical (zuuid->self);
    }

    static NAN_METHOD (export) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_export (zuuid->self);
    }

    static NAN_METHOD (eq) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_eq (zuuid->self);
    }

    static NAN_METHOD (neq) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_neq (zuuid->self);
    }

    static NAN_METHOD (dup) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_dup (zuuid->self);
    }

    static NAN_METHOD (test) {
        Zuuid *zuuid = Nan::ObjectWrap::Unwrap <Zuuid> (info.Holder ());
        zuuid_test (zuuid->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

    zuuid_t *self;
    public:
        zuuid_t *get_self () {
            return self;
        }
};

class Zauth: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zauth").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zauth").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zauth () {
        }
        ~Zauth () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zauth *zauth;
        zauth = new Zauth ();
        if (zauth) {
            zauth->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zauth *zauth = Nan::ObjectWrap::Unwrap <Zauth> (info.Holder ());
        zauth_test (zauth->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zbeacon: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zbeacon").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zbeacon").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zbeacon () {
        }
        ~Zbeacon () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zbeacon *zbeacon;
        zbeacon = new Zbeacon ();
        if (zbeacon) {
            zbeacon->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zbeacon *zbeacon = Nan::ObjectWrap::Unwrap <Zbeacon> (info.Holder ());
        zbeacon_test (zbeacon->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zgossip: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zgossip").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zgossip").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zgossip () {
        }
        ~Zgossip () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zgossip *zgossip;
        zgossip = new Zgossip ();
        if (zgossip) {
            zgossip->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zgossip *zgossip = Nan::ObjectWrap::Unwrap <Zgossip> (info.Holder ());
        zgossip_test (zgossip->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zmonitor: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zmonitor").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zmonitor").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zmonitor () {
        }
        ~Zmonitor () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zmonitor *zmonitor;
        zmonitor = new Zmonitor ();
        if (zmonitor) {
            zmonitor->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zmonitor *zmonitor = Nan::ObjectWrap::Unwrap <Zmonitor> (info.Holder ());
        zmonitor_test (zmonitor->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zproxy: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zproxy").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zproxy").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zproxy () {
        }
        ~Zproxy () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zproxy *zproxy;
        zproxy = new Zproxy ();
        if (zproxy) {
            zproxy->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zproxy *zproxy = Nan::ObjectWrap::Unwrap <Zproxy> (info.Holder ());
        zproxy_test (zproxy->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zrex: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zrex").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zrex").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zrex () {
        }
        ~Zrex () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zrex *zrex;
        zrex = new Zrex ();
        if (zrex) {
            zrex->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zrex *zrex = Nan::ObjectWrap::Unwrap <Zrex> (info.Holder ());
        zrex_test (zrex->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};

class Zsys: public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT (Init) {
            Nan::HandleScope scope;

            // Prepare constructor template
            Local <FunctionTemplate> tpl = Nan::New <FunctionTemplate> (New);
            tpl->SetClassName (Nan::New ("Zsys").ToLocalChecked ());
            tpl->InstanceTemplate ()->SetInternalFieldCount (1);

            // Prototypes
            Nan::SetPrototypeMethod (tpl, "test", test);

            constructor ().Reset (Nan::GetFunction (tpl).ToLocalChecked ());
            Nan::Set (target, Nan::New ("Zsys").ToLocalChecked (),
            Nan::GetFunction (tpl).ToLocalChecked ());
        }
    private:
        explicit Zsys () {
        }
        ~Zsys () {
        }

    static NAN_METHOD (New) {
        assert (info.IsConstructCall ());
        Zsys *zsys;
        zsys = new Zsys ();
        if (zsys) {
            zsys->Wrap (info.This ());
            info.GetReturnValue ().Set (info.This ());
        }
    }

    static NAN_METHOD (test) {
        Zsys *zsys = Nan::ObjectWrap::Unwrap <Zsys> (info.Holder ());
        zsys_test (zsys->self);
    }

    static Nan::Persistent <Function> & constructor () {
        static Nan::Persistent <Function> my_constructor;
        return my_constructor;
    }

};


extern "C" NAN_MODULE_INIT (czmq_initialize)
{
    Zactor::Init (target);
    Zarmour::Init (target);
    Zcert::Init (target);
    Zcertstore::Init (target);
    Zchunk::Init (target);
    Zclock::Init (target);
    Zconfig::Init (target);
    Zdigest::Init (target);
    Zdir::Init (target);
    ZdirPatch::Init (target);
    Zfile::Init (target);
    Zframe::Init (target);
    Zhash::Init (target);
    Zhashx::Init (target);
    Ziflist::Init (target);
    Zlist::Init (target);
    Zlistx::Init (target);
    Zloop::Init (target);
    Zmsg::Init (target);
    Zpoller::Init (target);
    Zproc::Init (target);
    Zsock::Init (target);
    Zstr::Init (target);
    Ztrie::Init (target);
    Zuuid::Init (target);
    Zauth::Init (target);
    Zbeacon::Init (target);
    Zgossip::Init (target);
    Zmonitor::Init (target);
    Zproxy::Init (target);
    Zrex::Init (target);
    Zsys::Init (target);
}

NODE_MODULE (czmq, czmq_initialize)
