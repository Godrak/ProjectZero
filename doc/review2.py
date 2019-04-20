# Copyright ReportLab Europe Ltd.2000 - 2016
# see license.txt for license details
__doc__ = ''
__version__ = '3.3.0'

# REPORTLAB_TEST_SCRIPT
import sys
from reportlab.platypus import *
from reportlab.lib.styles import getSampleStyleSheet
from reportlab.rl_config import defaultPageSize
from reportlab.lib.fonts import addMapping
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont

FONT = 'Play'
pdfmetrics.registerFont(TTFont(FONT, 'Play-Regular.ttf'))
addMapping(FONT, 0, 0, FONT)

PAGE_HEIGHT = defaultPageSize[1]
PAGE_WIDTH = defaultPageSize[0]

styles = getSampleStyleSheet()

Title = "Project Report 2"

from reportlab.lib.units import inch

pageinfo = "%s" % (Title)


def pageStyle(canvas, doc):
    # canvas.drawImage("snkanim.gif", 36, 36)
    canvas.saveState()
    # canvas.setStrokeColorRGB(1, 0, 0)
    # canvas.setLineWidth(5)
    # canvas.line(66, 72, 66, PAGE_HEIGHT - 72)
    canvas.setFont(FONT, 9)
    canvas.drawString(inch, 0.75 * inch, "Page %d %s" % (doc.page, pageinfo))
    canvas.restoreState()


def go():
    Elements.insert(0, Spacer(0, inch))
    doc = SimpleDocTemplate('report2.pdf')
    doc.build(Elements, onFirstPage=pageStyle, onLaterPages=pageStyle)


Elements = []

TitleStyle = styles["Heading1"]
styles["Heading1"].fontName = FONT
HeaderStyle = styles["Heading2"]
styles["Heading2"].fontName = FONT
ParagraphStyle = styles["Normal"]
styles["Normal"].fontName = FONT
SecondaryStyle = styles["Code"]
styles["Code"].fontName = FONT


def title(txt):
    return p(txt, style=TitleStyle, sep=0.5)


def header(txt):
    return p(txt, style=HeaderStyle, sep=0.3)


def p(txt, style=ParagraphStyle, paragraphClass=Paragraph, sep=0.1):
    s = Spacer(0.2 * inch, sep * inch)
    Elements.append(s)
    paragraph = paragraphClass(txt, style)
    Elements.append(paragraph)


def secondary(txt):
    s = Spacer(0.1 * inch, 0.1 * inch)
    Elements.append(s)
    p = Preformatted(txt, SecondaryStyle)
    Elements.append(p)


def image(path):
    f = open(path, 'rb')
    i = Image(f)
    i._restrictSize(PAGE_WIDTH * 0.9, PAGE_HEIGHT * 0.9 / 2)
    Elements.append(i)


title("Implementation of deferred snow deformation technique used in the Rise of the Tomb Raider")

header("Improved physics of the spheres")
p("""\
I have improved the behavior of the dynamic objects, they used to stutter and fall through the terrain a little.
They are now moving mostly downhills and have some sense of gravity and velocity. It is not a perfect 
physical simulation, but the movement is smooth and good enough for the purpose of this project.
""") 

header("Snow rendering")
p("""\
The rendering of snow was actually pretty easy, I reused most of the terrain setup and shaders. I just reimplemented
the tesselation evaluation shader and moved the snow a little bit higher than the ground, and gave it a white color.
However, I hit a problem called z-buffer fighting. This happens when there are multiple planes/triangles, that 
hit the same number in the z-buffer after projecting into clip space. I solved the problem by pushing the near camera 
plane further away from the camera spot, which increased the buffer precision by lowering the range mapped into it.
""") 

header("z-buffer fighting")
image("zb1.png")
image("zb2.png")


header("Writing into deformation texture")
p("""\
The method that I am implementing requires atomic writes into texture. Since the paper doesn't show much about the technical
stuff and the few bits that are there are for DX11, I had to figure this out myself. The OpenGL documentation does mention
atomic operation for images, but it is very sparse in terms of the necessary stuff that has to be done. For example,
it didn't explicitly mention that for atomic operations support, I need to use 'image' uniform type instead of 
'sampler'. But the most troublesome part was, that for 'image' uniform types, not only the texture has to be bound, but also a 
single layer of that texture has to be bound into so called image unit by special call 'glBindImageTexture'. I burnt several hours 
figuring out why the writing and reading to/from my deformation texture doesn't work, and finally discovered this command by luck.
I wonder where are these preconditions written, because I really couldn't find them...
""") 

p("""\
Once the deformation texture become usable, it was time to write the deformation according to the descriptions from the paper.
The hardest part of this step weren't actually the bit shifts to store the two numbers in single uint. I spent more time
mapping the texture correctly onto the ground around the camera, with configurable resolution, so that I can tune this parameter later.
This step is almost done, but the texture is now moving with the camera, so the deformation traces in the snow are moving as well,
which is obviously wrong. Other than this, the writing and reading from the texture works fine. There is however a second part that 
I haven't implemented yet, and that is the snow elevation along the trails.
""") 

header("Distance units refactoring")
p("""\
The OpenGL supports only 32 bit integer type for atomic operations, so I had to store the two deformation values into 16 bit 
values. This however meant that the deformation was rounded to the nearest integer, and that didn't work well with my 
mapping: 1 numeric unit = 1 meter. So I refactored the measure units to achieve configurable units per meter ratio.
""") 

header("Following steps")
p("""\
The first thing to do now is to make the mapping of deformation texture to terrain work properly, so that the snow trails don't move
together with the texture. Next step is probably going to be shading implementation, because I am currently hitting a performance drops
with OpenGL polygon mode and the visibility of the stuff is also not enough for the details like snow elevation.
""") 


header("""\
deformation - white represents the deformation texture mapping, the yellow is outside the deformation texture so no deformation is 
happening""")
image("def1.png")
header("")
image("def2.png")

go()
