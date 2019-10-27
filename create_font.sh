#!/bin/bash

usage() { echo "Usage: $0 [-h] [-i <charset>] [-m <width,height>] [-s <size>] [-r <range>] [-t <sdf|msdf>] [-p <padding>] font" 1>&2; exit 1; }

while getopts "hi:m:s:r:t:p:" opt; do
    case "${opt}" in
        i)
            charset_file=${OPTARG}
            ;;
        m)
            texture_size=${OPTARG}
            ;;
        s)
            font_size=${OPTARG}
            ;;
        r)
            distance_range=${OPTARG}
            ;;
        t)
            field_type=${OPTARG}
            [ "${field_type}" == 'sdf' ] || [ "${field_type}" == 'msdf' ] || usage
            ;;
        p)
            texture_padding=${OPTARG}
            ;;
        h|*)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${@}" ]; then
    usage
fi

if [ -z "${font_size}" ]; then
    font_size='42'
fi

if [ -z "${texture_size}" ]; then
    texture_size='512,256'
fi

if [ -z "${distance_range}" ]; then
    distance_range='3'
fi

if [ -z "${field_type}" ]; then
    field_type='sdf'
fi

if [ -z "${texture_padding}" ]; then
    texture_padding='1'
fi

font="${@##*/}"
font="${font%%.*}"
name="${font// /-}"
name="$(echo $name | tr '[:upper:]' '[:lower:]')"

texture="${name}-${font_size}-font.png"
bmfont="${font}.fnt"
blob="${name}.font"

if [ -d "assets/font" ]; then
    texture="assets/font/${texture}"
    bmfont="assets/font/${bmfont}"
    blob="assets/font/${blob}"
fi

if [ -z "${charset_file}" ]; then
    msdf-bmfont -o "${texture}" -m "${texture_size}" -s "${font_size}" -r "${distance_range}" -p "${texture_padding}" -t "${field_type}" "${@}"
else
    msdf-bmfont -o "${texture}" -m "${texture_size}" -s "${font_size}" -r "${distance_range}" -p "${texture_padding}" -t "${field_type}" -i "${charset_file}" "${@}"
fi

convert-bmfont "${bmfont}" --format bin | blob_font --stdin --bmfont -n -tr > "${blob}"
rm "${bmfont}"
