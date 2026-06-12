/* app.js — bst-library playground.
 *
 * Drives a JS port of the library and renders the tree in SVG.
 * The tree is re-laid-out on every change (in-order traversal
 * assigns x positions, depth assigns y).
 */
'use strict';

const tree = bst.create(
  (a, b) => (a > b) - (a < b),
  null
);

const SVG_NS = 'http://www.w3.org/2000/svg';
const NODE_R = 18;
const COL_W  = 50;   /* horizontal space per leaf */
const ROW_H  = 70;   /* vertical space per depth */

const elSvg   = document.getElementById('pg-tree');
const elOut   = document.getElementById('pg-output');
const elSize  = document.getElementById('pg-size');
const elHeight= document.getElementById('pg-height');

/* ---------- output buffer ---------- */
const outBuf = [];
function out(line, kind) {
  outBuf.push({ line, kind });
  if (outBuf.length > 50) outBuf.shift();
  elOut.innerHTML = outBuf
    .map(o => `<span class="${o.kind || ''}">${escapeHtml(o.line)}</span>`)
    .join('\n');
  elOut.scrollTop = elOut.scrollHeight;
}
function escapeHtml(s) {
  return String(s).replace(/[&<>"']/g, c => ({
    '&': '&amp;', '<': '&lt;', '>': '&gt;',
    '"': '&quot;', "'": '&#39;',
  }[c]));
}

/* ---------- layout ---------- */
function layout(node, x, y) {
  if (node === null) return { width: 0, left: x, right: x };
  const leftWidth  = layout(node.left,  x, y + ROW_H);
  const cx         = leftWidth.right + COL_W / 2;
  const rightWidth = layout(node.right, cx + COL_W / 2, y + ROW_H);
  node._x = cx; node._y = y + NODE_R + 20;
  return {
    width: rightWidth.right - leftWidth.left,
    left:  leftWidth.left,
    right: rightWidth.right,
  };
}

/* ---------- render ---------- */
function render() {
  /* Recompute layout. */
  const metrics = layout(tree.root, 20, 0);
  const width  = Math.max(metrics.width + 40, 600);
  const height = Math.max(tree.height * ROW_H + 60, 200);

  elSvg.setAttribute('viewBox', `0 0 ${width} ${height}`);
  elSvg.setAttribute('width', width);
  elSvg.setAttribute('height', height);
  while (elSvg.firstChild) elSvg.removeChild(elSvg.firstChild);

  /* Edges first, so circles sit on top. */
  const edges = document.createElementNS(SVG_NS, 'g');
  edges.setAttribute('stroke', '#a8a29e');
  edges.setAttribute('stroke-width', '1.5');
  edges.setAttribute('fill', 'none');
  walkEdges(tree.root, edges);
  elSvg.appendChild(edges);

  /* Nodes. */
  const nodes = document.createElementNS(SVG_NS, 'g');
  walkNodes(tree.root, nodes);
  elSvg.appendChild(nodes);

  elSize.textContent   = bst.size(tree);
  elHeight.textContent = bst.height(tree);
}

function walkEdges(n, g) {
  if (n === null) return;
  if (n.left)  { line(g, n._x, n._y, n.left._x,  n.left._y);  walkEdges(n.left,  g); }
  if (n.right) { line(g, n._x, n._y, n.right._x, n.right._y); walkEdges(n.right, g); }
}
function line(g, x1, y1, x2, y2) {
  const l = document.createElementNS(SVG_NS, 'line');
  l.setAttribute('x1', x1); l.setAttribute('y1', y1);
  l.setAttribute('x2', x2); l.setAttribute('y2', y2);
  g.appendChild(l);
}

function walkNodes(n, g) {
  if (n === null) return;
  walkNodes(n.left, g);
  walkNodes(n.right, g);
  /* Circle. */
  const c = document.createElementNS(SVG_NS, 'circle');
  c.setAttribute('cx', n._x);
  c.setAttribute('cy', n._y);
  c.setAttribute('r', NODE_R);
  c.setAttribute('fill', '#faf8f3');
  c.setAttribute('stroke', '#7c2d12');
  c.setAttribute('stroke-width', '1.5');
  g.appendChild(c);
  /* Label. */
  const t = document.createElementNS(SVG_NS, 'text');
  t.setAttribute('x', n._x);
  t.setAttribute('y', n._y);
  t.setAttribute('dy', '0.35em');
  t.setAttribute('text-anchor', 'middle');
  t.setAttribute('font-family', 'ui-monospace, Menlo, monospace');
  t.setAttribute('font-size', '14');
  t.setAttribute('font-weight', '600');
  t.setAttribute('fill', '#0c0a09');
  t.textContent = n.key;
  g.appendChild(t);
}

/* ---------- control wiring ---------- */
let nextValue = 1;

function insert(v) {
  if (bst.find(tree, v) !== null) {
    out(`insert ${v}: already present`, 'err');
    return;
  }
  bst.insert(tree, v, v);
  out(`insert ${v}`, 'ok');
  render();
}
function find(v) {
  const p = bst.find(tree, v);
  out(p !== null ? `find ${v}: found, payload=${p}` : `find ${v}: not found`,
      p !== null ? 'ok' : 'err');
}
function remove(v) {
  const ok = bst.remove(tree, v);
  out(ok ? `remove ${v}` : `remove ${v}: not present`,
      ok ? 'ok' : 'err');
  render();
}
function walk() {
  const seq = bst.inorder(tree);
  out('inorder: ' + (seq.length ? seq.join(' ') : '(empty)'), 'ok');
}

/* Numpad: each click inserts. */
document.querySelectorAll('.pg-numpad button[data-n]').forEach(b => {
  b.addEventListener('click', () => insert(parseInt(b.dataset.n, 10)));
});
/* Action buttons. */
document.getElementById('pg-find').addEventListener('click', () => {
  const v = parseInt(document.getElementById('pg-input').value, 10);
  if (Number.isNaN(v)) { out('enter a number', 'err'); return; }
  find(v);
});
document.getElementById('pg-remove').addEventListener('click', () => {
  const v = parseInt(document.getElementById('pg-input').value, 10);
  if (Number.isNaN(v)) { out('enter a number', 'err'); return; }
  remove(v);
});
document.getElementById('pg-walk').addEventListener('click', walk);
document.getElementById('pg-clear').addEventListener('click', () => {
  bst.clear(tree);
  outBuf.length = 0;
  elOut.innerHTML = '';
  render();
});

/* Initial render (empty tree). */
render();
out('bst-library playground ready', 'ok');
out('click a number to insert', '');
