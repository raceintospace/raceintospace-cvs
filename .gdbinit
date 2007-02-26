###############################################################################
# Electric Fence
#
# Debian's Electric Fence package provides efence as a shared library, which is
# very useful.
###############################################################################

define efence
        set environment EF_PROTECT_BELOW 0
        set environment LD_PRELOAD /usr/lib/libefence.so.0.0
        echo Enabled Electric Fence\n
end
document efence
Enable memory allocation debugging through Electric Fence (efence(3)).
        See also nofence and underfence.
end


define efence-under
        set environment EF_PROTECT_BELOW 1
        set environment LD_PRELOAD /usr/lib/libefence.so.0.0
        echo Enabled Electric Fence for undeflow detection\n
end
document efence-under
Enable memory allocation debugging for underflows through Electric Fence 
(efence(3)).
        See also nofence and efence.
end


define efence-off
        unset environment LD_PRELOAD
        echo Disabled Electric Fence\n
end
document efence-off
Disable memory allocation debugging through Electric Fence (efence(3)).
end
