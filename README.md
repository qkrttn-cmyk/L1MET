# L1 PUPPI MET

<p align="center">
    <img src=".structure.png" width="50%">
</p>

- `METAlgo` Architecture

This block receives 128 PUPPI candidates, and produces a MET object.

## 1. Setting up the workspace
### Connect
ssh STUDENTID@xxx.xxx.xx.xx

### Setup additional image stores
```
mkdir .config/containers

cat > ~/.config/containers/storage.conf <<EOF
[storage]
driver = "overlay"

[storage.options]
additionalimagestores = [ "/var/lib/localcontainers"]

[storage.options.overlay]
mount_program = "/usr/bin/fuse-overlayfs"
mountopt = "squash_to_root"
EOF
```
### Check image
podman images

### Connect to server through RDP
* For mac rdp: use redirection server name:i:1 (resolution 정하기)
echo $DISPLAY
* To allow x11 fowarding to display. Need to do inside rdp session.
xhost +local:STUDENTID

### vivado
```
podman run -e DISPLAY --rm -v
$HOME:$HOME -v /opt/XilinxLicense:/opt/XilinxLicense:ro --net=host
--device /dev/dri --name vivado_2023.2_app -it vivado_2023.2 /bin/bash
-c "export XILINXD_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic &&
export LM_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic && source
/opt/Xilinx/Vivado/2023.2/settings64.sh && cd $PWD && vivado"
```
or add this in .bashrc
```
alias vivado232='podman run -e DISPLAY --rm -v $HOME:$HOME -v /opt/XilinxLicense:/opt/XilinxLicense:ro --net=host --device /dev/dri --name vivado_2023.2_app -it vivado_2023.2 /bin/bash -c "export XILINXD_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic && export LM_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic && source /opt/Xilinx/Vivado/2023.2/settings64.sh && cd $PWD && vivado"'

function vitis_hls232 {
  local workdir="$PWD"

  podman run --rm -it \
    -e XILINXD_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic \
    -e LM_LICENSE_FILE=/opt/XilinxLicense/Xilinx.lic \
    -v /opt/XilinxLicense:/opt/XilinxLicense:ro \
    -v "$HOME:$HOME" \
    -v "$workdir:$workdir" -w "$workdir" \
    --net=host --device /dev/dri \
    --name vivado_2023.2_app \
    vivado_2023.2 \
    /bin/bash -lc 'source /opt/Xilinx/Vivado/2023.2/settings64.sh; exec vitis_hls "$@"' -- "$@"
}
```

### 7. Compare Hardware Output with CMSSW Reference

Finally, the testbench compares between RTL result and CMSSW L1 MET emulator output.
```
vitis_hls -f run_Sim.tcl "{nevents=1000 write=0 read=1 debug=0}"
```
