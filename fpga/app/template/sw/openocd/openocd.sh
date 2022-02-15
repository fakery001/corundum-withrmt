#!/bin/sh

VENDOR_ID=1234

# Determine PCI Domain:Bus:Device.Function for Corundum
# (Note that this assumes single-device in host.)
PCI_NODE=`lspci -d1234: -D | awk '{ print $1 }'`

if [ -z ${PCI_NODE} ]; then
  echo "Corundum PCIe endpoint not found. Check $0 for vendor ID $VENDOR_ID."
  exit
fi

echo Corundum FPGA board found at $PCI_NODE.

# This must be the VexRiscv fork of OpenOCD if targetting the VexRiscv based SoCs (Saxon, Murax, Briey, ...)

echo "OpenOCD requires root privileges to access PCIe extended capability."
echo
sudo local/bin/openocd -c "set CPU0_YAML ./cpu0.yaml" -c "set TAP_NAME xcu.tap" -c "set PCI_NODE $PCI_NODE" -f ./xvc-openocd.cfg -f ./vexriscv.cfg
