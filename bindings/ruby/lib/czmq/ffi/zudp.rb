################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################

module CZMQ
  module FFI

    #
    # @note This class is 100% generated using zproject.
    class Zudp
      # Raised when one tries to use an instance of {Zudp} after
      # the internal pointer to the native object has been nullified.
      class DestroyedError < RuntimeError; end

      # Boilerplate for self pointer, initializer, and finalizer
      class << self
        alias :__new :new
      end
      # Attaches the pointer _ptr_ to this instance and defines a finalizer for
      # it if necessary.
      # @param ptr [::FFI::Pointer]
      # @param finalize [Boolean]
      def initialize(ptr, finalize = true)
        @ptr = ptr
        if @ptr.null?
          @ptr = nil # Remove null pointers so we don't have to test for them.
        elsif finalize
          @finalizer = self.class.create_finalizer_for @ptr
          ObjectSpace.define_finalizer self, @finalizer
        end
      end
      # @param ptr [::FFI::Pointer]
      # @return [Proc]
      def self.create_finalizer_for(ptr)
        Proc.new do
          ptr_ptr = ::FFI::MemoryPointer.new :pointer
          ptr_ptr.write_pointer ptr
          ::CZMQ::FFI.zudp_destroy ptr_ptr
        end
      end
      # @return [Boolean]
      def null?
        !@ptr or @ptr.null?
      end
      # Return internal pointer
      # @return [::FFI::Pointer]
      def __ptr
        raise DestroyedError unless @ptr
        @ptr
      end
      # So external Libraries can just pass the Object to a FFI function which expects a :pointer
      alias_method :to_ptr, :__ptr
      # Nullify internal pointer and return pointer pointer.
      # @note This detaches the current instance from the native object
      #   and thus makes it unusable.
      # @return [::FFI::MemoryPointer] the pointer pointing to a pointer
      #   pointing to the native object
      def __ptr_give_ref
        raise DestroyedError unless @ptr
        ptr_ptr = ::FFI::MemoryPointer.new :pointer
        ptr_ptr.write_pointer @ptr
        __undef_finalizer if @finalizer
        @ptr = nil
        ptr_ptr
      end
      # Undefines the finalizer for this object.
      # @note Only use this if you need to and can guarantee that the native
      #   object will be freed by other means.
      # @return [void]
      def __undef_finalizer
        ObjectSpace.undefine_finalizer self
        @finalizer = nil
      end

      # Create UDP socket of the given type. Set the reuse option if
      # you need to reuse the socket's address.
      # @param type [Integer, #to_int, #to_i]
      # @param reuse [Boolean]
      # @return [CZMQ::Zudp]
      def self.new(type, reuse)
        type = Integer(type)
        reuse = !(0==reuse||!reuse) # boolean
        ptr = ::CZMQ::FFI.zudp_new(type, reuse)
        __new ptr
      end

      # Close a UDP socket
      #
      # @return [void]
      def destroy()
        return unless @ptr
        self_p = __ptr_give_ref
        result = ::CZMQ::FFI.zudp_destroy(self_p)
        result
      end

      # Send zframe to UDP socket to given address and port,
      # return -1 if sending failed (i.e. due to interface having
      # disappeared (happens easily with WiFi))
      #
      # @param frame [Zframe, #__ptr]
      # @param address [String, #to_s, nil]
      # @param port [Integer, #to_int, #to_i]
      # @return [Integer]
      def sendto(frame, address, port)
        raise DestroyedError unless @ptr
        self_p = @ptr
        frame = frame.__ptr if frame
        port = Integer(port)
        result = ::CZMQ::FFI.zudp_sendto(self_p, frame, address, port)
        result
      end

      # Receive zframe from UDP socket, and set address of peer that sent it
      # The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
      # NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.
      #
      # @param peername [::FFI::Pointer, #to_ptr]
      # @param peerlen [Integer, #to_int, #to_i]
      # @return [Zframe]
      def recv(peername, peerlen)
        raise DestroyedError unless @ptr
        self_p = @ptr
        peerlen = Integer(peerlen)
        result = ::CZMQ::FFI.zudp_recv(self_p, peername, peerlen)
        result = Zframe.__new result, false
        result
      end

      # Bind a socket to a and address and port number.
      # On failure, returns -1.
      #
      # @param address [String, #to_s, nil]
      # @param port [Integer, #to_int, #to_i]
      # @return [Integer]
      def bind(address, port)
        raise DestroyedError unless @ptr
        self_p = @ptr
        port = Integer(port)
        result = ::CZMQ::FFI.zudp_bind(self_p, address, port)
        result
      end

      # Return the socket's filedescriptor
      #
      # @return [Integer]
      def fd()
        raise DestroyedError unless @ptr
        self_p = @ptr
        result = ::CZMQ::FFI.zudp_fd(self_p)
        result
      end

      # Handle an I/O error on some socket operation; will report and die on
      # fatal errors, and continue silently on "try again" errors.
      #
      # @param reason [String, #to_s, nil]
      # @return [void]
      def self.error(reason)
        result = ::CZMQ::FFI.zudp_error(reason)
        result
      end

      # Self test of this class.
      #
      # @param verbose [Boolean]
      # @return [void]
      def self.test(verbose)
        verbose = !(0==verbose||!verbose) # boolean
        result = ::CZMQ::FFI.zudp_test(verbose)
        result
      end
    end
  end
end

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
