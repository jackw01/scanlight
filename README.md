# A Better Light Source For Scanning Color Negative Film

![](images/scanlight_header.jpg)

Left: [scanlight v4](./v4.md), right: [big scanlight](./big.md)

<small>Originally published 2024-08-05, last updated 2026-08-02</small>

## tl;dr

Narrowband trichromatic (RGB) light sources are superior to broadband (white) light for scanning color negative film; there are very good reasons why all commercial film scanners use RGB. ([film scanners were using blue LEDs at least a decade before they would be widely used in consumer products!](https://patents.google.com/patent/JPH09130546A/en?q=(film+scanner+LED)&oq=film+scanner+LED&sort=old)) Scanning film with an RGB light source produces high quality results with minimal post-processing, without the need for specialized software.

### the results

![](images/comparison4.jpg)

<small>Both scans captured using Fujifilm X-T30. White light scan captured using 95+ CRI 5000K light source. RGB scan captured using custom 450nm+525nm+665nm light source.</small>

## the simplified explanation

Color negative (C-41) film stores an image using cyan, magenta, and yellow dyes. Dyes appear a certain color because they absorb some wavelengths of light; for example, yellow dye mostly absorbs light in the 400-550nm range (which we perceive as violet through green), while allowing other wavelengths (yellow through red) to pass through the film. **These dyes are not intended to produce a human-viewable image, but rather to attenuate certain wavelengths of light for making prints on photosensitive paper.**

<img src="images/film_spectra.png" width="360">

White light sources emit light over a broad spectrum of wavelengths. When white light passes through C-41 film, some of these wavelengths are only slightly absorbed by the dyes, which means that these wavelengths aren't useful for recovering the information stored in the individual dye layers - for example, the 580-620nm band (yellow through red-orange light) is mostly transmitted through the film, with a small amount being absorbed by both the magenta and cyan dye layers.

The quality of white light sources is measured with CRI, which represents how well a light source reproduces the true color of a set of standardized color swatches. CRI is effectively a weighted average of how much light a light source emits across different parts of the visible light spectrum, and only a few of these parts matter when it comes to extracting information from color film. **CRI doesn't take into account the deep red (>630nm) and deep blue (<480nm) parts of the spectrum, so as a result, many high-CRI LEDs don't emit much light at these wavelengths, which happen to be where the yellow and cyan dyes absorb the most light.**

<small>note: Standardized scores that take into account these wavelength bands at the edges of the visible spectrum do exist (R9 and R12), but are not readily provided by all LED manufacturers.</small>

<img src="images/white_light.png" width="360">

With darkroom printing, this is not an issue, as RA-4 color print paper (and motion picture print film) are, by design, most sensitive to specific bands of wavelengths that avoid the crosstalk regions between the dye layers in negative film, and the paper is exposed with an incandescent light source with a smooth emission spectrum across the entire visible range (400-700nm) and beyond.

<img src="images/film_and_paper_spectra.png" width="360">

What about scanning film with a digital camera? Digital camera sensors are designed to capture light in a way that allows for a faithful reproduction of the colors that humans would perceive (this is a gross oversimplification.) C-41 film was *not* designed to be directly viewed by humans.

<img src="images/digital_camera_sensitivity.png" width="360">

Not only is the camera sensitive to wavelengths of light that are effectively ignored by RA-4 paper, there is significant crosstalk between the color channel sensitivities of digital camera sensors: light in the ~580-600nm range is seen roughly equally by the red and green channels, and light in the ~480-500nm range is seen by both the green and blue channels.

The red channel of the scan will contain not just information from the red-blocking cyan dye, but also the green-blocking magenta dye. The green channel will contain information from all three dye layers. Additionally, the overall light transmission of C-41 film is heavily biased towards yellow and orange wavelengths due to the orange mask and the relatively large gap between the magenta and cyan dye absorption peaks, which means that **a lot** of light gets through that interferes with both the red and green channels in the camera sensor without carrying much useful information about the colors stored in the film.

<small>note: the orange color of C-41 film is often misunderstood to be the color of the plastic film base, or an orange tint evenly applied over the whole image. It is actually a corrective mask made up of blue- and green-blocking positive images which compensate for deficiencies in the red-sensing and green-sensing layers of the film. For a good explanation of how this works, see [this article](https://observablehq.com/@dorskyee/understanding-color-film). In practice, this means that less-exposed parts of the negative let through disproportionately more yellow, orange, and red light.</small>

<img src="images/white_light_scan.png" width="360">

So if you scan color negative film with white light, you can't just invert the colors and expect a good result. You need to do some math to try to recover the individual dye layers of the film from ambiguous input data. This can't be done in standard image editing software, which is why dedicated software for negative conversion exists. The level of crosstalk on each channel depends heavily on both the emission spectra of the light source and the spectral sensitivity of the camera, so the conversion process needs to be either calibrated, manually adjusted until it looks “right”, or deduced by the processing software with varying degrees of accuracy. The color depth of the resulting positive image will be relatively low because much of the information recorded by the camera is effectively useless yellow-orange light that has passed through the film without being significantly attenuated by any of the dyes.

To avoid these issues, (almost?) all commercial negative film scanners use a trichromatic narrowband light source with red, green, and blue LEDs to minimize crosstalk between the dye layers in the film and the color channels on the image sensor, and some use monochrome sensors to eliminate sensor crosstalk entirely. When doing it this way, no guesswork is needed to recover a usable image and it's really as simple as inverting the color channels and setting the color balance and black/white points. There is still some crosstalk between camera sensor channels, but now that the light source emits light in three narrow, independently controllable wavelength bands, it can be easily measured and compensated for, or it can be avoided completely by imaging the red, green, and blue channels separately.

<img src="images/white_vs_rgb_scan.png">

### what light source works the best?

When considering the limitations of digital camera sensors, the ideal wavelengths are >650nm for red and <450nm for blue, as shown in the diagram above. Ideally, the green wavelength is 540-560nm to match the absorption peak of the magenta dye, but LEDs in that range are just not practical (see FAQ below for more details), and the standard 525nm works very well.

The next best alternative is to use general purpose RGB LEDs, which typically use 620-630nm for red and 460-480nm for blue. The results will not be the absolute best possible, but will still be far better than with white light. The Fuji Frontier SP3000, a popular professional scanner, uses 630nm for red and 465nm for blue ([source](https://discuss.pixls.us/t/input-color-profile-to-use-for-negatives/20271/13)), but it does not have to contend with the color channel sensitivity of a typical digital camera - it has a monochrome image sensor and combines three exposures taken with red, green, and blue light.

To get the best possible results, I designed my own custom RGB light sources using 665nm red and 455nm blue LEDs. More information on the designs can be found [here (scanlight v4)](./v4.md) and [here (big scanlight)](./big.md).

![](<images/scanlight_header.jpg>)

All design files for the light sources and film carriers, both current and older versions, can be downloaded from the [GitHub repository](https://github.com/jackw01/scanlight/).

### more sample scans

All scans were processed in [NegPy](https://github.com/marcinz606/NegPy), leaving all settings at the default values. Sensor calibration was used for the RGB scans. The groups of four negatives were scanned and manually processed together in order to demonstrate how the light source affects the overall look of the scans rather than to show a subjectively ideal edit for each photo.

![](images/scan_comparison_ektar_1_1.jpg)

<p align="center"><small>Kodak Ektar 100 (left: white light, right: narrowband RGB light)</small></p>

![](images/scan_comparison_fuji200_1_1.jpg)

<p align="center"><small>Fuji 200 (left: white light, right: narrowband RGB light)</small></p>

![](images/scan_comparison_ektar_3_1.jpg)

<p align="center"><small>Kodak Ektar 100 (left: white light, right: narrowband RGB light)</small></p>

![](images/scan_comparison_vision3_1_1.jpg)

<p align="center"><small>Kodak Vision3 500T (left: white light, right: narrowband RGB light)</small></p>

![](images/scan_comparison_fuji200_4_1.jpg)

<p align="center"><small>Fuji 200 (left: white light, right: narrowband RGB light)</small></p>

![](images/scan_comparison_fuji200_3_1.jpg)

<p align="center"><small>Fuji 200 (left: white light, right: narrowband RGB light)</small></p>

### recommended reading, if you're curious

* Edward J Giorgianni and Thomas E Madden, *Digital Color Management: Encoding Solutions, 2nd Edition*
* Robert L Shanebrook, *Making Kodak Film*

## part 2: responses to frequent questions and comments

<small>Updated 2026-08-02</small>

#### how should i process my scans?
As of 2026, free and open-source film scan processing software exists ([NegPy](https://github.com/marcinz606/NegPy)) which produces excellent results with little effort when inverting single-exposure RGB scans. NegPy uses a processing pipeline which emulates the RA-4 printing process, and supports flat-field correction and automatic merging of trichromatic (separate exposures for red, green, and blue) scans. I would highly recommend this to anyone who is looking to start scanning film at home or is having difficulty with their current processing workflow.

Using software specifically designed for processing white light scans (e.g. Negative Lab Pro) to process RGB scans may give suboptimal results. It's also possible to get good results without using any specialized software, by manually inverting your scans in photo editing software as described below. Manual inversion is much easier with RGB scans than with white light scans, but it still takes more effort than NegPy, and compensating for camera sensor crosstalk may be difficult depending on which image editing software you use.

#### but can't i just edit the white light scan to look like the RGB scan?
You can't get the exact same colors as the RGB scan by adjusting the white balance or individual color channels of a white light scan. This is physically impossible because the light interacted with the film in a fundamentally different way in the white light scan vs. the RGB scan before it was reduced to three bins of wavelengths approximately representing red, green, and blue by the camera sensor; some of the color information stored in the film is lost in this process.

#### but i keep seeing/hearing that using RGB light results in oversaturation or hue shifts
Increasing saturation is often done on purpose to produce a visually striking and not entirely accurate demonstration of why RGB is better (in practice, the differences in color reproduction are more subtle once saturation is equalized), or is the result of processing RGB scans with processing workflows intended to produce normal-looking results with white light scans. No one is saying you can’t adjust saturation in post, or use processing software that produces less-saturated results. Hue shifts are a result of using a processing workflow that doesn't compensate for camera sensor channel crosstalk, and should be a complete non-issue if using the sensor calibration tool in NegPy.

![](images/calibration_comparison.jpg)

<p align="center"><small>From left to right: RGB scan without sensor calibration (note blue-to-cyan and orange-to-magenta shifts); RGB scan with sensor calibration; Noritsu HS-1800 lab scan</small></p>

#### but the colors in the sample scans still don't look good to me / the colors don't look the way i think film should look
Getting the desired colors from C-41 film, whether by darkroom printing or scanning and editing, has always involved some level of manual adjustment. It is also worth noting that our collective idea of how film is supposed to look is often influenced by highly edited or low-quality scans; old, faded prints; or even software emulations. **Photography is art, and as such it has always involved making subjective decisions about how images should look; all photos are interpretations of raw data and it's up to the photographer to decide what interpretation looks best.** The best starting point for editing is a scan that represents the image data stored in the dye layers of the film with maximum dynamic range and minimal crosstalk.

#### won't this make all film look the same?
Scanning film with RGB light does not neutralize the differences in the "look" of different films; actually, it should produce results that are closer to the intended look of the film when printed on RA-4 paper. The differences in look come from differences in the film's sensitivity to light during exposure and chemical reactions that affect how the dyes are formed during development, not the absorption characteristics of the dyes themselves.

#### should i capture three separate exposures for each color channel?
You can either capture a single image with the red, green, and blue channels on at the same time or capture separate images for each color and merge them later. Both methods can produce satisfactory results. The latter approach has the potential for more technically accurate colors as it eliminates any chance of crosstalk between color channels, but takes significantly more work than capturing a single RGB image which can be processed directly.

When capturing red, green, and blue at the same time, the severity of crosstalk depends heavily on the spectral response of the camera's image sensor and camera manufacturer's color profile, and it may not even be noticeable in many cases. Crosstalk typically manifests as consistent color shifts (usually, reds appearing too pink or greens appearing too yellow,) and can usually be resolved by post-processing with software that supports a crosstalk matrix or RGB primary calibration, such as NegPy (open-source) or Lightroom.

#### how should i adjust the red/green/blue channel levels on the light source?
The red, green, and blue channels on the light source should be adjusted to maximize how much of the camera's dynamic range is utilized on all three color channels. To do this, set the camera white balance to neutral/5000K and adjust the light source so that the histogram peaks on each channel become aligned when the camera is viewing the light through an unexposed, developed piece of film. The deep red (~650nm) and deep blue (~450nm) wavelengths used for RGB scanning are near the edge of what most Bayer-filter image sensors are sensitive to, so some cameras will be significantly more sensitive to one of these wavelengths than others. Therefore, the optimal settings will differ between camera manufacturers and models.

#### should i use narrowband light for scanning color positive (slide) film?
Some people have had success using an adjustable RGB light source for recovering faded slides, but I would not recommend using a narrowband light source for scanning slides that are in good condition. The results I have seen so far are not promising compared to using white light, as colors are typically oversaturated and pink, red, and orange hues become indistinguishable from each other. This likely all comes down to the fact that color positive film is designed to reproduce colors naturally when viewed with a broad-spectrum light source. Any high CRI (95+) white light source should work well. There is some debate over the ideal color temperature: the majority of slide projectors used halogen bulbs with a 3000K-3500K color temperature, but some will argue that a 5000K color temperature should be used instead (this seems to be because Fujifilm datasheets recommend viewing with a ISO 3664 compliant viewer, which historically would have used specialized 5000K high-CRI fluorescent tubes as a light source). I haven't seen a side-by-side comparison, but I would assume that the difference in results would be very subtle or nonexistent if the white balance is corrected when editing the scans.

#### does light source / color matter for scanning black and white film?
No. White and narrowband light sources work equally well for scanning B&W film.

#### what are your thoughts on film carriers?
I have received quite a few emails echoing the sentiment that the film carriers on the market now are generally overpriced for what they are, and there seems to be little correlation between price and user experience. Most film carriers made for DSLR scanning guide the film through a narrow slot with an S-shaped curve on each end to keep it flat. For 35mm film, this seems to generally work well, and based on the reviews and comments I have seen I am not convinced that out of the many film carriers using this design, any perform significantly better than the others. For 120 film, the s-curve design struggles with film flatness when scanning the end frames on cut pieces of film, or if the film is curled across its width. There are a few alternative designs out there that should be able to provide superior film flatness in this case, but I haven't yet seen a thorough quantitative comparison of these. Some of these also involve rough 3D-printed parts coming into direct contact with the film emulsion, which doesn't seem ideal.

##### what about full-border scanning?
Film carriers that allow full-border scanning tend to be significantly worse at keeping the film flat and handling curled film, except for ones that hold the film against glass or acrylic, but those come with other issues (high cost, difficulty keeping the glass clean, Newton's rings, etc.)

#### what lens should i use?
[This list](https://www.pixl-latr.com/how-good-a-macro-lens-do-you-really-need-for-dslr-mirrorless-camera-scanning/) is generally a good starting point. The Micro-Nikkor 55mm f/2.8 AI is my personal recommendation; it outperforms many modern lenses in field flatness, corner-to-corner sharpness, and chromatic aberration, and it is very affordable.

I would **not** recommend low-cost third-party macro lenses like the TTArtisan 40mm f/2.8 Macro or 7artisans 60mm f/2.8 Macro II. While the build quality of these lenses is excellent and they generally have superior contrast to vintage lenses due to better optical coatings, in my experience they cannot match the top tiers of lenses in the above list in corner-to-corner sharpness and lack of chromatic aberration. I would also recommend against using extension tubes with modern lenses; this tends to result in field curvature and other aberrations which can be quite severe depending on the lens.

#### does brightness enhancing film actually work? should i use it?
Brightness enhancing film (BEF) does indeed work to make diffuse light sources more directional, but this can cause other issues. Collimated light makes the image on the film appear slightly sharper, but also exaggerates the appearance of dust and scratches. Depending on the relative positioning of the light source, film, and lens; and type of lens used; grid or moiré patterns may appear due to the structure of the brightness enhancing film and vignetting may occur or become more pronounced. BEF also tends to be difficult to keep clean of dust and fingerprints.

#### what about using an LCD/OLED display as the light source?
OLED displays do contain separate red, green, and blue emitters, but they typically have broader emission spectra than inorganic LEDs, and the red and blue peak wavelengths tend to be closer to 620nm and 470nm respectively. They will produce results with less crosstalk than a white light source, but not as good as RGB LEDs.

Older LCD displays generally emit white light that has been filtered through red, green, and blue filters and recombined. The results will depend on the emission spectrum of the exact display used, and will generally be no better than a white light source. Newer high-gamut LCDs (especially quantum dot LED LCDs) use backlight LEDs with special phosphors designed to emit in narrower bands than typical white LEDs, so they should in theory work as well as OLED.

#### 540-560nm appears to be the ideal green wavelength, why use 525nm?
LEDs in the 540-560nm range are impractical for most uses due to poor efficiency and thus are not widely available. Some GaP green LEDs exist which have peak emission at 555nm, but they are extremely inefficient, requiring hundreds to match the light output of a single 2835 mid-power InGaN LED. Some manufacturers do offer 530-535nm wavelength bins of their InGaN mid-power green LEDs, but these are difficult to source as they are commercially undesirable due to lower efficiency. Applications where a "true green" or "lime green" color is desired usually now use phosphor-converted LEDs, which are very efficient but are suboptimal for film scanning as they output light over a much broader band of wavelengths (60-100nm FWHM vs 15-30nm for direct green LEDs.)

##### how did old film scanners get ~540nm?
They either used cold-cathode fluorescent lamps with phosphors tuned to emit at the desired wavelengths, or used InGaN green LEDs from a higher wavelength bin. These have become increasingly hard to find as manufacturing processes have become more consistent and the industry has settled on ~525nm as the standard green wavelength for lighting and display applications. On the other hand, mid- and high-power ~450nm deep blue and ~660nm deep red LEDs have stayed available alongside their more common ~470nm and ~620nm counterparts due to increased demand, mostly in horticultural applications.

#### why does big scanlight have infrared LEDs?
The IR LEDs are for dust removal – the dyes in color negative film transmit infrared light, but dust does not. By capturing a separate IR image with an IR-sensitive camera, dust particles on the image can be easily located and inpainted. I originally added the LEDs for a specific customer who uses a specialized industrial camera and custom software processing pipeline to do this. As of June 2026, the only free/open-source software I know of that can automate this process is [NegPy](https://github.com/marcinz606/NegPy), although I haven't yet tested it with output from big scanlight.

#### will there be a desktop app for scanlight?
There are no plans for a desktop app right now, since the easiest way to make one by far is to just repackage a web app as a desktop app and there is little to be gained by doing this. It is possible to install the app as a progressive web app on most platforms, which will make it appear as a desktop app and allow it to be used without an internet connection.

#### what if i want to manually invert my scans?
It's definitely much easier to do this with narrowband scans than white light scans, but I still recommend using NegPy due to its automatic analysis and camera sensor calibration abilities.

##### 1. scan
Scan all frames as RAW using fixed white balance and exposure. Exposure should be set such that none of the color channels are clipping.
##### 2. import
Import scans into image editing software. Use a linear RAW profile. Apply lens corrections and use Lens Cast Calibration in Capture One or Flat-Field Correction in Lightroom to compensate for any unevenness in the light source if necessary.
##### 3. d-min balance
Neutralize the color of the minimum density (unexposed) areas of one negative by adjusting the red, green, and blue channel max levels.
##### 4. inversion
Invert the black and white points using the Levels or Curves tool and apply an s-shaped tone curve to adjust contrast. Use the brightness/gamma/Levels tool midpoint controls to adjust the overall scene brightness without affecting the black and white points.
##### 5. fine tuning
Apply the same adjustments to all scans from the same roll of film. Manually fine-tune the black and white points and brightness/gamma for all scans.
##### 6. crosstalk compensation
Use either the primary calibration panel in Lightroom, or the color tools in Capture One (more difficult), correct any hue shifts in the scans. Since hue shifts occur due to crosstalk between camera sensor channels in the light source emission bands, not any property of the film itself, this adjustment should be performed across a broad sample of images and saved for future use.

## license

The PCB schematic, layout, and Gerber files and the 3D CAD files for this project are released under the [CERN Open Hardware Licence Version 2 - Weakly Reciprocal](https://choosealicense.com/licenses/cern-ohl-w-2.0/) (CERN-OHL-W V2). Software and firmware are released under the MIT License. The text and illustrations in this article may not be reproduced without permission.
