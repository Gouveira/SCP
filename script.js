(function () {
    /* ---- Title: letter-by-letter drop ---- */
    var titleText = 'Smart House';
    var titleEl = document.getElementById('mainTitle');
    var baseDelay = 700; 

    for (var i = 0; i < titleText.length; i++) {
        var span = document.createElement('span');
        if (titleText[i] === ' ') {
            span.className = 'letter-space';
            span.style.width = '10px';
        } else {
            span.className = 'letter';
            span.textContent = titleText[i];
            span.style.animationDelay = (baseDelay + i * 60) + 'ms';
        }
        titleEl.appendChild(span);
    }

    /* ---- Subtitle: typewriter ---- */
    var subText = 'Panel de Control';
    var subEl = document.getElementById('subTitle');
    var subBaseDelay = baseDelay + titleText.length * 60 + 300;

    for (var j = 0; j < subText.length; j++) {
        var ch = document.createElement('span');
        if (subText[j] === ' ') {
            ch.className = 'char-space';
            ch.style.width = '6px';
        } else {
            ch.className = 'char';
            ch.textContent = subText[j];
            ch.style.animationDelay = (subBaseDelay + j * 55) + 'ms';
            ch.style.opacity = '0';
            ch.style.animation = 'typeChar 0.06s ease-out forwards';
        }
        subEl.appendChild(ch);
    }

    /* ---- Status dots: sequential pop ---- */
    var dotDelay = subBaseDelay + subText.length * 55 + 200;
    var dots = [
        document.getElementById('dot1'),
        document.getElementById('dot2'),
        document.getElementById('dot3')
    ];
    dots.forEach(function (dot, idx) {
        dot.style.animationDelay = (dotDelay + idx * 150) + 'ms';
        setTimeout(function () {
            dot.classList.add('live');
        }, dotDelay + idx * 150 + 500);
    });

    /* ---- Grid items: staggered card reveal ---- */
    var gridDelay = dotDelay + 600;
    var items = document.getElementById('gridBotones').children;
    for (var k = 0; k < items.length; k++) {
        (function (el, delay) {
            setTimeout(function () {
                el.classList.add('revealed');
                el.style.opacity = '1';
                el.style.transform = 'translateY(0)';
            }, delay);
        })(items[k], gridDelay + k * 80);
    }
})();
